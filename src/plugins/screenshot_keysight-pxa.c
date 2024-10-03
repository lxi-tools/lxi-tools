/*
 * Copyright (c) 2024  Primoz Salic
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

#define IMAGE_SIZE_MAX 0x400000 // 4 MB

int keysight_pxa_screenshot(char *address, char *id, int timeout)
{
    char* response = malloc(IMAGE_SIZE_MAX);
    char *command, *image;
    int device, length, n;
    char c;

    UNUSED(id);

    // Connect to LXI instrument
    device = lxi_connect(address, 0, NULL, timeout, VXI11);
    if (device == LXI_ERROR)
    {
        error_printf("Failed to connect\n");
        goto error_connect;
    }

    command = ":MMEM:STOR:SCR:THEM TDC";
    lxi_send(device, command, strlen(command), timeout);
    // Send SCPI commands to grab image
    command = ":MMEM:STOR:SCR \'C:\\Windows\\Temp\\sa.png\'";
    lxi_send(device, command, strlen(command), timeout);
    command = ":MMEM:DATA? \'C:\\Windows\\Temp\\sa.png\'";
    lxi_send(device, command, strlen(command), timeout);
    length = lxi_receive(device, response, IMAGE_SIZE_MAX, timeout);
    if (length < 0)
    {
        error_printf("PXA: Failed to receive message\n");
        goto error_receive;
    }

    // Strip IEEE 488.2 Data Block header
    c = response[1];
    n = atoi(&c);
    image = &response[0];
    image += n+2;
    length -= n+2;

    // Strip ending newline
    length--;

    // Dump remaining image data to file
    screenshot_file_dump(image, length, "png");

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
struct screenshot_plugin keysight_pxa =
{
    .name = "keysight-pxa",
    .description = "Keysight PXA Spectrum Analyzer",
    .regex = "Agilent Keysight Technologies N90. A,",
    .screenshot = keysight_pxa_screenshot
};
