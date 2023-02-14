/*
 * Copyright (c) 2023  Martin Lund
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

#define IMAGE_SIZE_MAX 0x800000 // 8 MB

int tektronix_screenshot_mso_5(char *address, char *id, int timeout)
{
    char* response = malloc(IMAGE_SIZE_MAX);
    char *command;
    int device, length;

    UNUSED(id);

    // Connect to LXI instrument
    device = lxi_connect(address, 0, NULL, timeout, VXI11);
    if (device == LXI_ERROR)
    {
        error_printf("Failed to connect\n");
        goto error_connect;
    }

    // Send SCPI commands to grab PNG image
    command = "SAVE:IMAGE 'c:/lxi-tools-screenshot.png'";
    lxi_send(device, command, strlen(command), timeout);
    command = "*WAI";
    lxi_send(device, command, strlen(command), timeout);
    command = "FILESYSTEM:READFILE 'c:/lxi-tools-screenshot.png'";
    lxi_send(device, command, strlen(command), timeout);
    length = lxi_receive(device, response, IMAGE_SIZE_MAX, timeout);
    if (length < 0)
    {
        error_printf("Failed to receive message\n");
        goto error_receive;
    }

    // Cleanup
    command = "FILESystem:DELEte 'c:/lxi-tools-screenshot.png'";
    lxi_send(device, command, strlen(command), timeout);
    command = "*WAI";
    lxi_send(device, command, strlen(command), timeout);

    // Dump PNG image data to file
    screenshot_file_dump(response, length, "png");

    // Free allocated memory for screenshot
    free(response);

    // Disconnect
    lxi_disconnect(device);

    return 0;

error_connect:
error_receive:

    // Free allocated memory for screenshot
    free(response);

    return 1;
}


// Screenshot plugin configuration
struct screenshot_plugin tektronix_mso_5 =
{
    .name = "tektronix-mso-5",
    .description = "Tektronix MSO 5 series oscilloscope",
    .regex = "TEKTRONIX MSO5...",
    .screenshot = tektronix_screenshot_mso_5
};
