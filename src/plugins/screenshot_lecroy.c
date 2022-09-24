/*
 * Copyright (c) 2022  Perry Hung
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

#define IMAGE_SIZE_MAX       0x400000   // 4 MB
#define MIN_TRANSFER_SIZE    32

// Poll until the SCDP bit is clear
static int scdp_status_wait(device, timeout) {
  char *command;
  char response[6];

  for (unsigned retry = 0; retry < 5; retry++) {
    command = "INR?";
    lxi_send(device, command, strlen(command), timeout);
    int rc = lxi_receive(device, response, MIN_TRANSFER_SIZE, timeout);
    if (rc < (int)sizeof(response)) {
      printf("INR? receive failed\n");
      return 1;
    }

    // Parse the mask, screendump is bit 2
    unsigned long state = strtoul(&response[4], NULL, 10);
    if (state & 2) {
      return 0;
    }
  }

  return 1;
}

int lecroy_screenshot(char *address, char *id, int timeout)
{
    UNUSED(id);

    char* response;
    response = malloc(IMAGE_SIZE_MAX);
    if (!response)
    {
        error_printf("Memory allocation failed\n");
        return 1;
    }

    int device = lxi_connect(address, 0, NULL, timeout, VXI11);
    if (device == LXI_ERROR)
    {
        error_printf("Failed to connect\n");
        goto error_connect;
    }

    // Clear status registers and enable screen dump mask bit, then do a read to
    // clear any pending bits
    char * command = "*CLS";
    lxi_send(device, command, strlen(command), timeout);
    command = "INE 2";
    lxi_send(device, command, strlen(command), timeout);
    scdp_status_wait(device, timeout);

    // Delete any existing file, if any. This ensures that the autoincrementing
    // suffix counter is reset to zero.
    command = "DELF DISK,HDD,FILE,'D:\\HardCopy\\lxi-screenshot--00000.png'";
    lxi_send(device, command, strlen(command), timeout);

    // Set hardcopy to dump PNGs to a file
    command = "HCSU DEV,png,DEST,FILE,DIR,'D:\\HardCopy\',AREA,FULLSCREEN,FILE,'lxi-screenshot'";
    lxi_send(device, command, strlen(command), timeout);

    // Trigger screendump
    command = "SCDP";
    lxi_send(device, command, strlen(command), timeout);
    if (scdp_status_wait(device, timeout)) {
        printf("screendump bit not set?\n");
    }

    // Read it back
    command = "TRFL? DISK,HDD,FILE,'D:\\HardCopy\\lxi-screenshot--00000.png'";
    lxi_send(device, command, strlen(command), timeout);
    int length = lxi_receive(device, response, IMAGE_SIZE_MAX, timeout);
    if (length < MIN_TRANSFER_SIZE) {
      error_printf("receive error: %d\n", length);
      goto error_receive;
    }

    // Skip 'TRFL? #'
    length -= 6;

    // Parse the digit and skip IEEE header
    char digit = response[6];
    char *image = &response[7];
    switch (digit) {
    case '0' ... '9': {
        size_t n = digit - '0';
        image += n;
        length -= n;
        break;
    }
    default:
        break;
    }

    // Strip 8 byte CRC footer and 2 byte terminator
    length -= 10;

    if (length < 0 || length >= IMAGE_SIZE_MAX)
    {
        error_printf("Invalid message\n");
        goto error_receive;
    }

    screenshot_file_dump(image, length, "png");
    free(response);
    lxi_disconnect(device);

    return 0;

error_receive:
    lxi_disconnect(device);
error_connect:
    free(response);

    return 1;
}

// Screenshot plugin configuration
struct screenshot_plugin lecroy =
{
    .name = "lecroy-wp",
    .description = "LeCroy WavePro Oscilloscope",
    .regex = "LECROY WP LCRY",
    .screenshot = lecroy_screenshot
};
