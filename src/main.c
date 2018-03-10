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
#include "discover.h"
#include "scpi.h"
#include "screenshot.h"
#include "benchmark.h"
#include "run.h"
#include <lxi.h>

int main(int argc, char* argv[])
{
    int status = EXIT_SUCCESS;
    double result;

    // Parse options
    parse_options(argc, argv);

    // Initialize LXI library
    lxi_init();

    switch (option.command)
    {
        case DISCOVER:
            if (option.mdns)
                status = discover(true, option.timeout);
            else
                status = discover(false, option.timeout);
            break;
        case SCPI:
            if (option.interactive)
                status = enter_interactive_mode(option.ip, option.port, option.timeout, option.protocol);
            else if (option.run_script)
                status = run_script(option.ip, option.port, option.timeout, option.protocol, option.script_filename);
            else
                status = scpi(option.ip, option.port, option.timeout, option.protocol, option.scpi_command);
            break;
        case SCREENSHOT:
            screenshot_register_plugins();
            if (option.list)
            {
                screenshot_list_plugins();
                return 0;
            }
            status = screenshot(option.ip, option.plugin_name, option.screenshot_filename, option.timeout, true, NULL, NULL, NULL, NULL);
            break;
        case BENCHMARK:
            status = benchmark(option.ip, option.port, option.timeout, option.protocol, option.count, true, &result, NULL);
            break;
         case RUN:
            status = run(option.lua_script_filename, option.timeout);
            break;
   }

    return status;
}
