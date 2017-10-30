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
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include "screenshot.h"

#define PLUGIN_LIST_SIZE_MAX 50

extern struct screenshot_plugin rigol_1000z;
extern struct screenshot_plugin rigol_2000;
extern struct screenshot_plugin rigol_4000;
extern struct screenshot_plugin rs_hmo1000;

static struct screenshot_plugin *plugin_list[PLUGIN_LIST_SIZE_MAX] = { };

void screenshot_file_dump(void *data, int length, char *filename, char *format)
{
    FILE *fp;

    fp = fopen(filename, "w+");
    if (fp == NULL)
    {
        printf("Error: Could not write image file (%s)\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    fwrite(data, 1, length, fp);
    fclose(fp);

    printf("Saved %s screenshot image to %s\n", format, filename);
}

void screenshot_plugin_register(struct screenshot_plugin *plugin)
{
    int i = 0;

    // Find first available entry in plugin list
    while ((i < PLUGIN_LIST_SIZE_MAX) && (plugin_list[i] != NULL))
        i++;

    if (i >= PLUGIN_LIST_SIZE_MAX)
    {
        printf("Error: Screenshot plugin list full\n");
        exit(EXIT_FAILURE);
    }

    // Add plugin
    plugin_list[i] = plugin;
}

void screenshot_list_plugins(void)
{
    int i = 0;

    // Print list of available plugins
    printf("           Name   Description\n");
    while ((i < PLUGIN_LIST_SIZE_MAX) && (plugin_list[i] != NULL))
    {
        printf("%15s   %s\n", plugin_list[i]->name, plugin_list[i]->description);
        i++;
    }
}

void screenshot_register_plugins(void)
{
    // Register screenshot plugins here
    screenshot_plugin_register(&rigol_1000z);
    screenshot_plugin_register(&rigol_2000);
    screenshot_plugin_register(&rigol_4000);
    screenshot_plugin_register(&rs_hmo1000);
}

int screenshot(char *address, char *model, char *filename, int timeout)
{
    int i = 0;
    bool no_match = true;

    // Check for required options
    if (strlen(address) == 0)
    {
        printf("Error: Missing address\n");
        exit(EXIT_FAILURE);
    }

    if (strlen(model) == 0)
    {
        printf("Error: Missing model\n");
        exit(EXIT_FAILURE);
    }

    if (strlen(filename) == 0)
    {
        printf("Error: Missing filename\n");
        exit(EXIT_FAILURE);
    }

    // Find relevant screenshot plugin (match model to plugin name)
    while ((i < PLUGIN_LIST_SIZE_MAX) && (plugin_list[i] != NULL))
    {
        if (strcmp(plugin_list[i]->name, model) == 0)
        {
            no_match = false;
            break;
        }
        i++;
    }

    if (no_match)
    {
        printf("Error: Unknown model\n");
        exit(EXIT_FAILURE);
    }

    // Call screenshot function
    plugin_list[i]->screenshot(address, filename, timeout);

    // Return status result

    return 0;
}
