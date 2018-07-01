/*
 * Copyright (c) 2016-2017  Martin Lund
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holders nor contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT
 * HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "options.h"
#include "error.h"
#include <lxi.h>

#define RESPONSE_LENGTH_MAX 0x500000
#define ID_LENGTH_MAX 65536

static void hex_print(void *data, int length)
{
    char *bufferp;
    int i;

    bufferp = data;
    (void)bufferp;

    for (i=0; i<length; i++)
    {
        if ((i%10 == 0) && (i !=0 ))
        {
            printf("\n");
        }
        printf("0x%02x ", (unsigned char) bufferp[i]);
    }

    // Append newline if printing to tty terminal (not file)
    if (isatty(fileno(stdout)))
        printf("\n");
}

void strip_trailing_space(char *line)
{
    int i = strlen(line) - 1;

    while (i >= 0)
    {
        if ( isspace(line[i]) )
            line[i] = '\0';
        else
            break;
        i--;
    }
}

int question(const char *string)
{
    int i;

    for (i = 0; string[i] != '\0'; i++)
    {
        if (string[i] == '?')
            return true;
    }

    return false;
}

int scpi(char *ip, int port, int timeout, lxi_protocol_t protocol, char *command)
{
    char response[RESPONSE_LENGTH_MAX] = "";
    char command_buffer[1000];
    int device, length;;

    strip_trailing_space(command);

    if (protocol == RAW)
    {
        // Add newline to command string
        strcpy(command_buffer, command);
        command_buffer[strlen(command)] = '\n';
        command_buffer[strlen(command)+1] = 0;
        command = command_buffer;
    }

    // Connect
    device = lxi_connect(ip, port, NULL, timeout, protocol);
    if (device != LXI_OK)
    {
        error_printf("Unable to connect to LXI device\n");
        goto error_connect;
    }

    // Send SCPI command
    length = lxi_send(device, command, strlen(command), timeout);
    if (length < 0)
    {
        error_printf("Failed to send message\n");
        goto error_send;
    }

    // Only expect response in case we are firing a question command
    if (question(command))
    {
        length = lxi_receive(device, response, RESPONSE_LENGTH_MAX, timeout);
        if (length < 0)
        {
            error_printf("Failed to receive message\n");
            goto error_receive;
        }

        // Print response
        if (option.hex)
            hex_print(response, length);
        else
            {
                int i;
                for (i=0; i<length; i++)
                    putchar(response[i]);

                // Append newline if printing to tty terminal (not file)
                if ( isatty(fileno(stdout)) && (response[length-1] != '\n'))
                    printf("\n");
            }
    }

    // Disconnect
    lxi_disconnect(device);

    return 0;

error_send:
error_receive:

    // Disconnect
    lxi_disconnect(device);

error_connect:

    return 1;
}

int enter_interactive_mode(char *ip, int port, int timeout, lxi_protocol_t protocol)
{
    char response[RESPONSE_LENGTH_MAX] = "";
    int device, length;
    char *input = "";

    // Connect
    device = lxi_connect(ip, port, NULL, timeout, protocol);
    if (device != LXI_OK)
    {
        error_printf("Unable to connect to LXI device\n");
        goto error_connect;
    }

    printf("Connected to %s\n", ip);
    printf("Entering interactive mode (ctrl-d to quit)\n\n");

    // Enter line/command processing loop
    while (true)
    {
        input = readline("lxi> ");
        if (input == NULL)
            break;

        add_history(input);

        strip_trailing_space(input);

        // Skip empty lines
        if (strlen(input) == 0)
            continue;

        // Send entered input as SCPI command
        length = lxi_send(device, input, strlen(input), timeout);
        if (length < 0)
            error_printf("Failed to send message\n");

        // Only expect response in case we are firing a question command
        if (question(input))
        {
            length = lxi_receive(device, response, RESPONSE_LENGTH_MAX, timeout);
            if (length < 0)
            {
                error_printf("Failed to receive message\n");
            } else
            {
                // Make sure we terminate response string
                response[length] = 0;

                // Print response
                printf("%s", response);
            }
        }
    }

    printf("\n");

    // Disconnect
    lxi_disconnect(device);

    return 0;

error_connect:

    return 1;
}

int run_script(char *ip, int port, int timeout, lxi_protocol_t protocol, char *filename)
{
    FILE *fp;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    char response[RESPONSE_LENGTH_MAX] = "";
    int device, length, i;

    // Open script file
    fp = fopen(filename, "r");
    if (fp == NULL)
    {
        error_printf("Unable to open file %s\n", filename);
        goto error_fopen;
    }

    // Connect
    device = lxi_connect(ip, port, NULL, timeout, VXI11);
    if (device != LXI_OK)
    {
        error_printf("Unable to connect to LXI device\n");
        goto error_connect;
    }

    printf("Connected to %s\n", ip);
    printf("Running script %s\n\n", filename);

    while ((read = getline(&line, &len, fp)) != -1)
    {
        printf("%s", line);

        strip_trailing_space(line);

        // Skip empty lines
        if (strlen(line) == 1)
            continue;

        // Send read line as SCPI command
        length = lxi_send(device, line, strlen(line), timeout);
        if (length < 0)
            error_printf("Failed to send message\n");

        // Only expect response in case we are firing a question command
        if (line[strlen(line)-1] == '?')
        {
            length = lxi_receive(device, response, RESPONSE_LENGTH_MAX, timeout);
            if (length < 0)
            {
                error_printf("Failed to receive message\n");
            } else
            {
                // Make sure we terminate response string
                response[length] = 0;

                // Print response
                for (i=0; i<length; i++)
                putchar(response[i]);
            }
        }
    }

    free(line);
    fclose(fp);

    // Disconnect
    lxi_disconnect(device);

    return 0;

error_connect:
    fclose(fp);
error_fopen:
    return 1;
}
