/*
 * Copyright (c) 2016-2018  Martin Lund
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
#include "error.h"
#include <lxi.h>

static int device_count = 0;
static int service_count = 0;

static void broadcast(const char *address, const char *interface)
{
    printf("Broadcasting on interface %s\n", interface);
}

static void device(const char *address, const char *id)
{
    printf("  Found \"%s\" on address %s\n", id, address);
    device_count++;
}

static void service(const char *address, const char *id, const char *service, int port)
{
    printf("  Found \"%s\" on address %s\n    %s service on port %u\n", id, address, service, port);
    service_count++;
}

int discover(bool mdns, int timeout)
{
    lxi_info_t info;

    // Set up info callbacks
    info.broadcast = &broadcast;
    info.device = &device;
    info.service = &service;

    printf("Searching for LXI devices - please wait...\n\n");

    // Search for LXI devices / services
    if (mdns)
    {
        lxi_discover(&info, timeout, DISCOVER_MDNS);
        if (service_count == 0)
            printf("No services found\n");
        else
            printf("\nFound %d service%c\n", service_count, service_count > 1 ? 's' : ' ');
    }
    else
    {
        lxi_discover(&info, timeout, DISCOVER_VXI11);
        printf("\n");
        if (device_count == 0)
            printf("No devices found\n");
        else
            printf("Found %d device%c\n", device_count, device_count > 1 ? 's' : ' ');
    }

    printf("\n");

    return 0;
}
