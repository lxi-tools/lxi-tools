/*
 * Copyright (c) 2016, Martin Lund
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
#include <ctype.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "options.h"
#include <lxi.h>

void file_dump(void *data, int length, char *filename)
{
    FILE *fp;

    fp=fopen(filename, "w+");
    fwrite(data, 1, length, fp);
    fclose(fp);

    printf("Saved %d bytes to %s\n", length, filename);
}

void hex_dump(void *data, int length)
{
    int i;
    char *bufferp;

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
    printf("\n");
}

void strip_trailing_space(char *line)
{
    int i = strlen(line) - 1;

    while (i > 0)
    {
        if ( isspace(line[i]) )
            line[i] = '\0';
        else
            break;
        i--;
    }
}

int scpi(char *ip, char *command, int timeout, char *filename)
{
    char response[LXI_MESSAGE_LENGTH_MAX] = "";
    int device;
    int length;

    strip_trailing_space(command);

    device = lxi_connect(ip);
    lxi_send(device, command, strlen(command), timeout);

    // Only expect response in case we are firing a question command
    if (command[strlen(command)-1] == '?')
    {
        lxi_receive(device, response, &length, timeout);

        // Print response
        if (option.dump_hex)
            hex_dump(response, length);
        else if (option.dump_file)
            file_dump(response, length, filename);
        else
            printf("%s", response);
    }

    return 0;
}

int enter_interactive_mode(char *ip, int timeout)
{
    char response[LXI_MESSAGE_LENGTH_MAX] = "";
    char *input = "";
    int device;
    int length;

    device = lxi_connect(ip);
    printf("Connected to %s\n", ip);
    printf("Entering interactive mode - write q to quit\n");

    // Enter line/command processing loop
    while (strcmp(input, "q") != 0)
    {
        input = readline("lxi > ");
        if (input == NULL)
            break;

        add_history(input);

        strip_trailing_space(input);

        // Send entered input as SCPI command
        lxi_send(device, input, strlen(input), timeout);

        // Only expect response in case we are firing a question command
        if (input[strlen(input)-1] == '?')
        {
            lxi_receive(device, response, &length, timeout);

            // Print response
            printf("%s", response);
        }
    }

    printf("\n");
    return 0;
}

int run_script(char *ip, int timeout, char *filename)
{
    FILE *fp;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    char response[LXI_MESSAGE_LENGTH_MAX] = "";
    int device;
    int length;

    // Open script file
    fp = fopen(filename, "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);

    // Connect
    device = lxi_connect(ip);
    printf("Connected to %s\n", ip);
    printf("Running script %s\n", filename);

    while ((read = getline(&line, &len, fp)) != -1)
    {
        printf("%s", line);

        strip_trailing_space(line);

        // Send read line as SCPI command
        lxi_send(device, line, strlen(line), timeout);

        // Only expect response in case we are firing a question command
        if (line[strlen(line)-1] == '?')
        {
            lxi_receive(device, response, &length, timeout);

            // Print response
            printf("%s", response);
        }
    }

    free(line);
    fclose(fp);

    return 0;
}

int discover(void)
{
    lxi_device_t device;
    lxi_devices_t *devices;

    printf("Searching for LXI devices - please wait...\n\n");

    lxi_discover_devices(&devices, option.timeout, true);

    printf("\n");

    if (devices == NULL)
    {
        printf("No LXI devices found\n\n");
        return 0;
    } else
        printf("Available devices:\n\n");

    // Print discovered device IP address and ID
    while (lxi_get_device_info(devices, &device) == 0)
    {
        printf("[%s]  %s\n", device.address, device.id);
    }

    return 0;
}

int main(int argc, char* argv[])
{
    int status = EXIT_SUCCESS;

    // Parse options
    parse_options(argc, argv);

    // Initialize LXI library
    lxi_init();

    switch (option.command)
    {
        case DISCOVER:
            status = discover();
            break;
        case SCPI:
            if (option.interactive)
                status = enter_interactive_mode(option.ip, option.timeout);
            else if (option.run_script)
                status = run_script(option.ip, option.timeout, option.filename);
            else
                status = scpi(option.ip, option.scpi_command, option.timeout, option.filename);
            break;
    }

    return status;
}
