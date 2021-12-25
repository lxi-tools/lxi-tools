/*
 * Copyright (c) 2019  Sam harry Tzavaras
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
#include <lxi.h>
#include "error.h"
#include "screenshot.h"

#define IMAGE_SIZE_MAX 308278 // 302KB
#define PARAM_STR_SIZE 10

typedef struct{
    char Header[PARAM_STR_SIZE];
    char Format[PARAM_STR_SIZE];
    char Compression[PARAM_STR_SIZE];
    char Layout[PARAM_STR_SIZE];
    char Port[PARAM_STR_SIZE];
}restore;

void length_check(int length);

int tektronix_screenshot_3000(char *address, int timeout)
{
    restore param;
    char response[IMAGE_SIZE_MAX];
    int length, device;
    int sock = 0, valread;
    char *command, command_str[100];

    // Connect to LXI instrument
    device = lxi_connect(address, 0, NULL, timeout, VXI11);
    if (device == LXI_ERROR)
    {
        error_printf("Failed to connect\n");
        return 1;
    }

    // Check the device
    command = "*IDN?";
    lxi_send(device, command, strlen(command), timeout);	
    length_check(lxi_receive(device, response, IMAGE_SIZE_MAX, timeout));
    if (strstr(response,"TDS 3") != NULL)
    {
        // Send SCPI commands to grab current image parameters and config for grab image
        command = "hardcopy:Format?";
        lxi_send(device, command, strlen(command), timeout);
        length_check(lxi_receive(device, param.Format, IMAGE_SIZE_MAX, timeout));
        command = "hardcopy:Format bmpc";
        lxi_send(device, command, strlen(command), timeout);

        command = "hardcopy:compression?";
        lxi_send(device, command, strlen(command), timeout);
        length_check(lxi_receive(device, param.Compression, IMAGE_SIZE_MAX, timeout));
        command = "hardcopy:compression off";
        lxi_send(device, command, strlen(command), timeout);

        command = "hardcopy:layout?";
        lxi_send(device, command, strlen(command), timeout);
        length_check(lxi_receive(device, param.Layout, IMAGE_SIZE_MAX, timeout));
        command = "hardcopy:layout Portrait";
        lxi_send(device, command, strlen(command), timeout);

        command = "hardcopy:Port?";
        lxi_send(device, command, strlen(command), timeout);
        length_check(lxi_receive(device, param.Port, IMAGE_SIZE_MAX, timeout));
        command = "hardcopy:Port gpib";
        lxi_send(device, command, strlen(command), timeout);

        // Send SCPI commands to grab image
        command = "hardcopy start";
        lxi_send(device, command, strlen(command), timeout);
        length = lxi_receive(device, response, IMAGE_SIZE_MAX, timeout);
        length_check(length);
        // Dump PNG image data to file
        screenshot_file_dump(response, length, "bmp");

        // Restore old configuration
        sprintf(command_str,"hardcopy:Format %s", param.Format);
        lxi_send(device, command_str, strlen(command_str), timeout);

        sprintf(command_str,"hardcopy:compression %s", param.Compression);
        lxi_send(device, command_str, strlen(command_str), timeout);

        sprintf(command_str,"hardcopy:layout %s", param.Layout);
        lxi_send(device, command_str, strlen(command_str), timeout);

        sprintf(command_str,"hardcopy:Port %s", param.Port);
        lxi_send(device, command_str, strlen(command_str), timeout);
    }
    else
        printf("Device doesn't match\n");

    // Disconnect
    lxi_disconnect(device);
    return 0;
}

void length_check(int length)
{
    if (length <= 0)
        error_printf("Failed to receive message\n");
}

// Screenshot plugin configuration
struct screenshot_plugin tektronix_3000 =
{
    .name = "tektronix-3000",
    .description = "Tektronix TDS 3000 series e*Scope oscilloscope (experimental)",
    .regex = "TEKTRONIX TDS3... ",
    .screenshot = tektronix_screenshot_3000
};
