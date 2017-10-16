/*
 * Copyright (c) 2016-2017, Martin Lund
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

#define IMAGE_SIZE_MAX 0x200000 // 2 MB

void file_dump(void *data, int length, char *filename)
{
    FILE *fp;

    fp = fopen(filename, "w+");
    fwrite(data, 1, length, fp);
    fclose(fp);
}

int capture_screenshot(char *address, char *filename, int timeout)
{
    char response[IMAGE_SIZE_MAX] = "";
    int device;
    int length;
    char *command = "display:data? on,0,png";
    char *response_p;
    char c;
    int n;

    device = lxi_connect(address);
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
    response_p = &response[0];
    response_p += n+2;
    length -= n+2;

    // Dump image data to file
    file_dump(response_p, length, filename);

    printf("Saved PNG screenshot to %s\n", filename);

    lxi_disconnect(device);

    return 0;
}

int main(int argc, char* argv[])
{
    int status = EXIT_SUCCESS;

    if (argc != 3)
    {
        printf("Usage: rigol_1000z_screenshot <address> <filename>\n");
        exit(EXIT_FAILURE);
    }

    // Initialize lxi library
    lxi_init();

    // Capture screenshot, 5 seconds timeout
    status = capture_screenshot(argv[1], argv[2], 5000);

    return status;
}
