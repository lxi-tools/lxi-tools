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
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include "screenshot.h"

#define PLUGIN_LIST_SIZE_MAX 50

extern struct screenshot_plugin keysight_iv2000x;
extern struct screenshot_plugin rigol_1000;
extern struct screenshot_plugin rigol_2000;
extern struct screenshot_plugin rs_hmo1000;
extern struct screenshot_plugin tektronix_2000;

static struct screenshot_plugin *plugin_list[PLUGIN_LIST_SIZE_MAX] = { };

char *date_time(void)
{
    static char date_time_string[50];

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(date_time_string, "%d-%02d-%02d_%02d:%02d:%02d", tm.tm_year + 1900,
            tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

    return date_time_string;
}

void screenshot_file_dump(void *data, int length, char *filename, char *format)
{
    char automatic_filename[80];
    char *screenshot_filename;
    FILE *fp;
    int i = 0;

    // Resolve screenshot filename
    if (strlen(filename) == 0)
    {
        sprintf(automatic_filename, "screenshot_%s.%s", date_time(), format);
        screenshot_filename = automatic_filename;
    }
    else
        screenshot_filename = filename;

    // Write screenshot file
    fp = fopen(screenshot_filename, "w+");
    if (fp == NULL)
    {
        printf("Error: Could not write screenshot file (%s)\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    fwrite(data, 1, length, fp);
    fclose(fp);

    printf("Saved screenshot image to %s\n", screenshot_filename);
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
    int i = 0, j = 0;
    int length, length_max = 0;

    // Find length of longest plugin name
    while ((i < PLUGIN_LIST_SIZE_MAX) && (plugin_list[i] != NULL))
    {
        length = strlen(plugin_list[i]->name);
        if (length_max < length)
            length_max = length;
        i++;
    }

    // Pretty print list of available plugins
    i=0;
    for (j=0; j<(length_max-4); j++)
        putchar(' ');
    printf("Name   Description\n");
    while ((i < PLUGIN_LIST_SIZE_MAX) && (plugin_list[i] != NULL))
    {
        for (j=0; j<(length_max-strlen(plugin_list[i]->name)); j++)
            putchar(' ');
        printf("%s", plugin_list[i]->name);
        printf("   %s\n", plugin_list[i]->description);
        i++;
    }
}

void screenshot_register_plugins(void)
{
    // Register screenshot plugins here
    screenshot_plugin_register(&keysight_iv2000x);
    screenshot_plugin_register(&rigol_1000);
    screenshot_plugin_register(&rigol_2000);
    screenshot_plugin_register(&rs_hmo1000);
    screenshot_plugin_register(&tektronix_2000);
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

    // Call capture screenshot function
    return plugin_list[i]->screenshot(address, filename, timeout);
}
