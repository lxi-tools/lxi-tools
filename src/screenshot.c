/*
 * Copyright (c) 2017-2018  Martin Lund
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
#include <regex.h>
#include "screenshot.h"
#include "error.h"
#include <lxi.h>

#define PLUGIN_LIST_SIZE_MAX 50
#define ID_LENGTH_MAX 65536

extern struct screenshot_plugin keysight_dmm;
extern struct screenshot_plugin keysight_ivx;
extern struct screenshot_plugin rigol_1000z;
extern struct screenshot_plugin rigol_2000;
extern struct screenshot_plugin rigol_dg4000;
extern struct screenshot_plugin rigol_dl3000;
extern struct screenshot_plugin rigol_dm3068;
extern struct screenshot_plugin rigol_dp800;
extern struct screenshot_plugin rigol_dsa;
extern struct screenshot_plugin rs_hmo_rtb;
extern struct screenshot_plugin siglent_sdm3000;
extern struct screenshot_plugin siglent_sdg;
extern struct screenshot_plugin siglent_sds;
extern struct screenshot_plugin siglent_ssa3000x;
extern struct screenshot_plugin tektronix_2000;

static struct screenshot_plugin *plugin_list[PLUGIN_LIST_SIZE_MAX] = { };
static char *screenshot_filename = NULL;
static char *screenshot_address = NULL;
static bool screenshot_no_gui;
static void *screenshot_image_buffer;
static int *screenshot_image_size;
static char *screenshot_image_format;
static char *screenshot_image_filename;

static int get_device_id(char *address, char *id, int timeout)
{
    int device, bytes_sent, bytes_received;
    char *command;

    // Connect to LXI instrument
    device = lxi_connect(address, 0, NULL, timeout, VXI11);
    if (device == LXI_ERROR)
    {
        error_printf("Failed to connect\n");
        goto error_connect;
    }

    // Get instrument ID
    command = "*IDN?";

    bytes_sent = lxi_send(device, command, strlen(command), timeout);
    if (bytes_sent < 0)
        goto error_send;

    bytes_received = lxi_receive(device, id, ID_LENGTH_MAX, timeout);
    if (bytes_received < 0)
    {
        error_printf("Failed to receive message\n");
        goto error_receive;
    }

    // Disconnect
    lxi_disconnect(device);

    // Remove trailing newline
    if (id[bytes_received-1] == '\n')
        id[bytes_received-1] = 0;

    return 0;

error_receive:
error_send:
    lxi_disconnect(device);
error_connect:
    return 1;
}

static bool regex_match(const char *string, const char *pattern)
{
    regex_t regex;
    int status;

    if (regcomp(&regex, pattern, REG_EXTENDED | REG_NOSUB) != 0)
        return false; // No match

    status = regexec(&regex, string, (size_t) 0, NULL, 0);
    regfree(&regex);

    if (status != 0)
        return false; // No match

    return true; // Match
}

static char *date_time(void)
{
    static char date_time_string[50];

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(date_time_string, "%d-%02d-%02d_%02d:%02d:%02d", tm.tm_year + 1900,
            tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

    return date_time_string;
}

void screenshot_file_dump(void *data, int length, char *format)
{
    char automatic_filename[1000];
    char *filename;
    char *image_data = data;
    int i = 0;
    FILE *fd;

    // Resolve screenshot output filename
    if (strlen(screenshot_filename) == 0)
    {
        // Automatically resolve screenshot filename if no filename is provided
        sprintf(automatic_filename, "screenshot_%s_%s.%s", screenshot_address, date_time(), format);
        filename = automatic_filename;
    }
    else
    {
        // Write image data to specified filename
        filename = screenshot_filename;
    }

    if (screenshot_no_gui)
    {
        if (strcmp(screenshot_filename, "-") == 0)
        {
            // Write image data to stdout in case filename is '-'
            for (i=0; i<length; i++)
                putchar(*(image_data+i));
            return;
        }
        else
        {
            // Write screenshot to file
            fd = fopen(filename, "w+");
            if (fd == NULL)
            {
                error_printf("Could not write screenshot file (%s)\n", strerror(errno));
                exit(EXIT_FAILURE);
            }
            fwrite(data, 1, length, fd);
            fclose(fd);

            printf("Saved screenshot image to %s\n", filename);
        }
    }
    else
    {
        // Write screenshot to buffer
        memcpy(screenshot_image_buffer, data, length);
        *screenshot_image_size = length;
        strcpy(screenshot_image_format, format);
        strcpy(screenshot_image_filename, filename);
    }
}

void screenshot_plugin_register(struct screenshot_plugin *plugin)
{
    int i = 0;

    // Find first available entry in plugin list
    while ((i < PLUGIN_LIST_SIZE_MAX) && (plugin_list[i] != NULL))
        i++;

    if (i >= PLUGIN_LIST_SIZE_MAX)
    {
        error_printf("Screenshot plugin list full\n");
        exit(EXIT_FAILURE);
    }

    // Add plugin
    plugin_list[i] = plugin;
}

void screenshot_list_plugins(void)
{
    int length, length_max = 0;
    int i = 0, j = 0;

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
    // Register screenshot plugins
    screenshot_plugin_register(&keysight_dmm);
    screenshot_plugin_register(&keysight_ivx);
    screenshot_plugin_register(&rigol_1000z);
    screenshot_plugin_register(&rigol_2000);
    screenshot_plugin_register(&rigol_dg4000);
    screenshot_plugin_register(&rigol_dl3000);
    screenshot_plugin_register(&rigol_dm3068);
    screenshot_plugin_register(&rigol_dp800);
    screenshot_plugin_register(&rigol_dsa);
    screenshot_plugin_register(&rs_hmo_rtb);
    screenshot_plugin_register(&siglent_sdm3000);
    screenshot_plugin_register(&siglent_sdg);
    screenshot_plugin_register(&siglent_sds);
    screenshot_plugin_register(&siglent_ssa3000x);
    screenshot_plugin_register(&tektronix_2000);
}

int screenshot(char *address, char *plugin_name, char *filename,
               int timeout, bool no_gui, void *image_buffer,
               int *image_size, char *image_format, char *image_filename)
{
    bool no_match = true;
    char id[ID_LENGTH_MAX];
    bool token_found = true;
    char *token = NULL;
    int plugin_winner = -1;
    int match_count = 0;
    int match_count_max = 0;
    char *regex_buffer;
    int i = 0;

    // Check parameters
    if (strlen(address) == 0)
    {
        error_printf("Missing address\n");
        exit(EXIT_FAILURE);
    }

    // Save variables
    screenshot_address = address;
    screenshot_filename = filename;
    screenshot_no_gui = no_gui;
    screenshot_image_buffer = image_buffer;
    screenshot_image_size = image_size;
    screenshot_image_format = image_format;
    screenshot_image_filename = image_filename;

    if (strlen(plugin_name) == 0)
    {
        // Get instrument ID
        if (get_device_id(address, id, timeout) != 0)
        {
            error_printf("Unable to retrieve instrument ID\n");
            exit(EXIT_FAILURE);
        }

        // Find relevant screenshot plugin (match instrument ID to plugin)
        while ((i < PLUGIN_LIST_SIZE_MAX) && (plugin_list[i] != NULL))
        {
            // Skip plugin if it has no .regex entry
            if (plugin_list[i]->regex == NULL)
            {
                i++;
                continue;
            }

            // Walk through space separated regular expressions in regex string
            regex_buffer = strdup(plugin_list[i]->regex);
            while (token_found == true)
            {
                if (token == NULL)
                    token = strtok(regex_buffer, " ");
                else
                    token = strtok(NULL, " ");

                if (token != NULL)
                {
                    // Match regular expression against ID
                    if (regex_match(id, token))
                        match_count++; // Successful match
                }
                else
                    token_found = false;
            }
            free(regex_buffer);

            // Plugin with most matches wins
            if (match_count > match_count_max)
            {
                plugin_winner = i;
                match_count_max = match_count;
            }

            // Reset
            match_count = 0;
            token_found = true;
            i++;
        }

        if (plugin_winner == -1)
        {
            error_printf("Could not autodetect which screenshot plugin to use - please specify plugin name manually\n");
            exit(EXIT_FAILURE);
        }

        if (isatty(fileno(stdout)) && screenshot_no_gui)
            printf("Loaded %s screenshot plugin\n", plugin_list[plugin_winner]->name);

        no_match = false;
        i = plugin_winner;
    }
    else
    {
        // Find relevant screenshot plugin (match specified plugin name to plugin)
        while ((i < PLUGIN_LIST_SIZE_MAX) && (plugin_list[i] != NULL))
        {
            if (strcmp(plugin_list[i]->name, plugin_name) == 0)
            {
                no_match = false;
                break;
            }
            i++;
        }
    }

    if (no_match)
    {
        error_printf("Unknown plugin name\n");
        exit(EXIT_FAILURE);
    }

    // Call capture screenshot function
    return plugin_list[i]->screenshot(address, timeout);
}
