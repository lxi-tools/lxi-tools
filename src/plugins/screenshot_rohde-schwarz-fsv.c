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

#define IMAGE_SIZE_MAX 0x100000 * 20 // 20 MB

int rs_fsv_screenshot(char *address, char *id, int timeout)
{
    char *command, *image;
    int device, length, n;
    char c;

    UNUSED(id);

    // Prepare response buffer
    char* response = malloc(IMAGE_SIZE_MAX);
    if (response == NULL)
    {
        error_printf("Failed to allocate response buffer\n");
        return 1;
    }

    // Connect to LXI instrument
    device = lxi_connect(address, 0, NULL, timeout, VXI11);
    if (device == LXI_ERROR)
    {
        error_printf("Failed to connect\n");
        goto error_connect;
    }

    // Send SCPI commands to grab image
    command = "HCOP:DEV:LANG PNG";
    lxi_send(device, command, strlen(command), timeout);
    command = "HCOP:CMAP:DEF4";
    lxi_send(device, command, strlen(command), timeout);
    command = "HCOP:DEST 'MMEM'";
    lxi_send(device, command, strlen(command), timeout);

    command = ":MMEMory:NAME 'C:\\R_S\\instr\\user\\Print.png'";
    lxi_send(device, command, strlen(command), timeout);
    command = ":HCOPy:IMMediate;*OPC";
    lxi_send(device, command, strlen(command), timeout);
    command = ":MMEMory:DATA? 'C:\\R_S\\instr\\user\\Print.png'";
    lxi_send(device, command, strlen(command), timeout);
    length = lxi_receive(device, response, IMAGE_SIZE_MAX, timeout);
    if (length < 0)
    {
        error_printf("Failed to receive message\n");
        goto error_receive;
    }

    // Cleanup
    command = ":MMEMory:DELete 'C:\\R_S\\instr\\user\\Print.png';*OPC";
    lxi_send(device, command, strlen(command), timeout);

    // Strip header
    c = response[1];
    n = atoi(&c);
    image = &response[0];
    image += n+2;
    length -= n+2;

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
struct screenshot_plugin rs_fsv =
{
    .name = "rs-fsv",
    .description = "Rohde & Schwarz FSV series Signal and Spectrum Analyzers",
    .regex = "Rohde&Schwarz FSV",
    .screenshot = rs_fsv_screenshot
};
