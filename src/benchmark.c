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

#define ID_LENGTH_MAX 65536

int benchmark(char *ip, int port, int timeout, lxi_protocol_t protocol, int count, bool no_gui, double *result, void (*progress)(void))
{
    struct timespec start, stop;
    double elapsed_time;
    int device, i, bytes_received;
    char id[ID_LENGTH_MAX];
    char *command = "*IDN?";

    // Check for required options
    if (strlen(ip) == 0)
    {
        error_printf("Missing address\n");
        exit(EXIT_FAILURE);
    }

    if (protocol == RAW)
        command = "*IDN?\n";

    // Connect
    device = lxi_connect(ip, port, NULL, timeout, protocol);
    if (device != LXI_OK)
    {
        error_printf("Unable to connect to LXI device\n");
        return 1;
    }

    if (no_gui)
        printf("Benchmarking by sending %d ID requests. Please wait...\n", count);

    // Start time
    if ( clock_gettime(CLOCK_MONOTONIC, &start) == -1 )
    {
        error_printf("Failed to get start time\n");
        return 1;
    }

    // Run benchmark
    for (i=0; i<count; i++)
    {
        // Get instrument ID
        lxi_send(device, command, strlen(command), timeout);
        bytes_received = lxi_receive(device, id, ID_LENGTH_MAX, timeout);
        if (bytes_received < 0)
        {
            error_printf("Failed to receive instrument ID\n");
            return 1;
        }

        if (no_gui)
        {
            // Print progress
            printf("\r%d", i+1);
            fflush(stdout);
        } else if (progress != NULL)
            progress();
    }

    // Stop time
    if( clock_gettime(CLOCK_MONOTONIC, &stop) == -1 )
    {
        error_printf("Failed to get stop time\n");
        return 1;
    }

    // Calculate elapsed time in seconds
    elapsed_time =
        (double)(stop.tv_sec - start.tv_sec) +
        (double)(stop.tv_nsec - start.tv_nsec)*1.0e-9;

    *result = count / elapsed_time;

    if (no_gui)
        printf("\rResult: %.1f requests/second\n", *result);

    // Disconnect
    lxi_disconnect(device);

    return 0;
}
