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

#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <getopt.h>
#include <termios.h>
#include "config.h"
#include "options.h"
#include "error.h"
#include <lxi.h>

struct option_t option =
{
    NO_COMMAND, // Default command
    3,          // Default timeout in seconds
    "",         // Default IP address
    "",         // Default SCPI command
    false,      // Default no hex dump
    false,      // Default no interactive mode
    false,      // Default no run script
    "",         // Default script filename
    "",         // Default screenshot plugin name
    false,      // Default no list
    "",         // Default screenshot filename
    VXI11,      // Default protocol
    5025,       // Default raw/TCP port (See http://www.lxistandard.org/About/LXI-Protocols.aspx)
};

void print_help(char *argv[])
{
    printf("Usage: %s [--version] [--help] <command> [<args>]\n", argv[0]);
    printf("\n");
    printf("  -v, --version                        Display version\n");
    printf("  -h, --help                           Display help\n");
    printf("\n");
    printf("Commands:\n");
    printf("  discover [<options>]                 Search for LXI devices\n");
    printf("  scpi [<options>] <scpi-command>      Send SCPI command\n");
    printf("  screenshot [<options>] [<filename>]  Capture screenshot\n");
    printf("\n");
    printf("Discover options:\n");
    printf("  -t, --timeout <seconds>              Timeout (default: %d)\n", option.timeout);
    printf("\n");
    printf("Scpi options:\n");
    printf("  -a, --address <ip>                   Device IP address\n");
    printf("  -t, --timeout <seconds>              Timeout (default: %d)\n", option.timeout);
    printf("  -x, --dump-hex                       Print response in hexadecimal\n");
    printf("  -i, --interactive                    Enter interactive mode\n");
    printf("  -s, --script <filename>              Run script file\n");
    printf("  -r, --raw                            Use raw/TCP\n");
    printf("  -p, --raw-port <port>                Use raw/TCP port (default: %d)\n", option.port);
    printf("\n");
    printf("Screenshot options:\n");
    printf("  -a, --address <ip>                   Device IP address\n");
    printf("  -t, --timeout <seconds>              Timeout (default: %d)\n", option.timeout*5);
    printf("  -p, --plugin <name>                  Use screenshot plugin by name\n");
    printf("  -l, --list                           List available screenshot plugins\n");
    printf("\n");
}

void print_version(void)
{
    printf("lxi v%s\n", VERSION);
}

void parse_options(int argc, char *argv[])
{
    int c;

    // Print help if no arguments provided
    if (argc == 1)
    {
        print_help(argv);
        exit(EXIT_SUCCESS);
    }

    // Convert default timeout to milliseconds
    option.timeout = option.timeout * 1000;

    // getopt_long stores the option index here
    int option_index = 0;

    // Skip ahead past command
    optind = 2;

    if (strcmp(argv[1], "discover") == 0)
    {
        option.command = DISCOVER;

        static struct option long_options[] =
        {
            {"timeout",        required_argument, 0, 't'},
            {0,                0,                 0,  0 }
        };

        /* Parse discover options */
        c = getopt_long(argc, argv, "t:", long_options, &option_index);

        while (c != -1)
        {
            switch (c)
            {
                case 't':
                    option.timeout = atoi(optarg) * 1000;
                    break;
                case '?':
                    exit(EXIT_FAILURE);
            }
            c = getopt_long(argc, argv, "t:", long_options, &option_index);
        }
    }
    else if (strcmp(argv[1], "scpi") == 0)
    {
        option.command = SCPI;

        static struct option long_options[] =
        {
            {"address",        required_argument, 0, 'a'},
            {"timeout",        required_argument, 0, 't'},
            {"dump-hex",       no_argument,       0, 'x'},
            {"interactive",    no_argument,       0, 'i'},
            {"script",         required_argument, 0, 's'},
            {"raw",            no_argument,       0, 'r'},
            {"raw-port",       required_argument, 0, 'p'},
            {0,                0,                 0,  0 }
        };

        do
        {
            /* Parse scpi options */
            c = getopt_long(argc, argv, "t:a:xis:rp:", long_options, &option_index);

            switch (c)
            {
                case 'a':
                    strncpy(option.ip, optarg, 500);
                    break;

                case 't':
                    option.timeout = atoi(optarg) * 1000;
                    break;

                case 'x':
                    option.dump_hex = true;
                    break;

                case 'i':
                    option.interactive = true;
                    break;

                case 's':
                    option.run_script = true;
                    option.script_filename = optarg;
                    break;

                case 'r':
                    option.protocol = RAW;
                    break;

                case 'p':
                    option.port = atoi(optarg);
                    break;

                case '?':
                    exit(EXIT_FAILURE);
            }
        } while (c != -1);
    } else if (strcmp(argv[1], "screenshot") == 0)
    {
        option.command = SCREENSHOT;

        // Increase default timeout for screenshots
        option.timeout *= 5;

        static struct option long_options[] =
        {
            {"address",        required_argument, 0, 'a'},
            {"timeout",        required_argument, 0, 't'},
            {"plugin",         required_argument, 0, 'p'},
            {"list",           no_argument,       0, 'l'},
            {0,                0,                 0,  0 }
        };

        do
        {
            /* Parse screenshot options */
            c = getopt_long(argc, argv, "a:t:p:l", long_options, &option_index);

            switch (c)
            {
                case 'a':
                    strncpy(option.ip, optarg, 500);
                    break;

                case 't':
                    option.timeout = atoi(optarg) * 1000;
                    break;

                case 'p':
                    option.plugin_name = optarg;
                    break;

                case 'l':
                    option.list = true;
                    break;

                case '?':
                    exit(EXIT_FAILURE);
            }
        } while (c != -1);
    } else
    {
        // No command provided so we restore index
        optind = 1;

        static struct option long_options[] =
        {
            {"version",        no_argument,       0, 'v'},
            {"help",           no_argument,       0, 'h'},
            {0,                0,                 0,  0 }
        };

        do
        {
            /* Parse options */
            c = getopt_long(argc, argv, "vh", long_options, &option_index);

            switch (c)
            {
                case 'v':
                    print_version();
                    exit(EXIT_SUCCESS);

                case 'h':
                    print_help(argv);
                    exit(EXIT_SUCCESS);

                case '?':
                    exit(EXIT_FAILURE);
            }
        } while (c != -1);
    }

    if ((option.command == NO_COMMAND) && (optind != argc))
    {
        error_printf("Unknown command\n");
        exit(EXIT_FAILURE);
    }

    if (option.command == SCPI)
    {
        if (optind != argc)
            strncpy(option.scpi_command, argv[optind++], 500);

        if (strlen(option.ip) == 0)
        {
            error_printf("No IP address specified\n");
            exit(EXIT_FAILURE);
        }

        if (strlen(option.scpi_command) == 0)
        {
            error_printf("No SCPI command specified\n");
            exit(EXIT_FAILURE);
        }
    }

    if ((option.command == SCREENSHOT) && (optind != argc))
    {
        strncpy(option.screenshot_filename, argv[optind++], 500);
    }

    /* Print any unknown arguments */
    if (optind < argc)
    {
        error_printf("Unknown arguments: ");
        while (optind < argc)
            fprintf(stderr, "%s ", argv[optind++]);
        fprintf(stderr, "\n");
        exit(EXIT_FAILURE);
    }
}
