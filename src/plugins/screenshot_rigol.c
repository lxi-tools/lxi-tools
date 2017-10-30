/*
 * Copyright (c) 2017, Martin Lund
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
#include "screenshot.h"

#define IMAGE_SIZE_MAX 0x400000 // 4 MB

int rigol_oscilloscope_screenshot(char *address, char *filename, int timeout)
{
    char response[IMAGE_SIZE_MAX];
    char *command, *image;
    int device, length, n;
    char c;

    // Connect to LXI instrument
    device = lxi_connect(address, NULL, timeout);
    if (device == LXI_ERROR)
    {
        printf("Error: Failed to connect\n");
        exit(1);
    }

    // Send SCPI command to grab PNG image
    command = "display:data? on,0,png";
    lxi_send(device, command, strlen(command), timeout);
    length = lxi_receive(device, response, IMAGE_SIZE_MAX, timeout);
    if (length < 0)
    {
        printf("Error: Failed to receive message\n");
        exit(1);
    }

    // Strip TMC block header
    c = response[1];
    n = atoi(&c);
    image = &response[0];
    image += n+2;
    length -= n+2;

    // Dump remaining PNG image data to file
    screenshot_file_dump(image, length, filename, "PNG");

    // Disconnect
    lxi_disconnect(device);

    return 0;
}


// Screenshot plugin configuration
struct screenshot_plugin rigol_1000z =
{
	.name = "rigol-1000z",
	.description = "Rigol 1000z series oscilloscopes",
	.screenshot = rigol_oscilloscope_screenshot
};

// Screenshot plugin configuration
struct screenshot_plugin rigol_2000 =
{
	.name = "rigol-2000",
	.description = "Rigol 2000 series oscilloscopes (experimental)",
	.screenshot = rigol_oscilloscope_screenshot
};

// Screenshot plugin configuration
struct screenshot_plugin rigol_4000 =
{
	.name = "rigol-4000",
	.description = "Rigol 4000 series oscilloscopes (experimental)",
	.screenshot = rigol_oscilloscope_screenshot
};
