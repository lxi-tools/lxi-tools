/*
 * Copyright (c) 2018-2022, Martin Lund
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

#include <unistd.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <lxi.h>
#include "error.h"
#include "misc.h"
#include <stdlib.h>

#define RESPONSE_LENGTH_MAX 0x400000
#define SESSIONS_MAX 1024
#define CLOCKS_MAX 1024

struct session_t
{
    int timeout;
    int protocol;
};

static struct session_t session[SESSIONS_MAX];

struct lua_clock_t
{
    double time_start;
    bool allocated;
};

static struct lua_clock_t lua_clock[CLOCKS_MAX];

static char log_script[] =
"__lxi_log_data = {}\n"
"\n"
"function lxi_log_new()\n"
"\n"
"    local data = __lxi_log_data\n"
"\n"
"    handle = #data + 1\n"
"    data[handle] = {}\n"
"\n"
"    return handle\n"
"end\n"
"\n"
"function lxi_log_free(handle)\n"
"--    table.remove(data, handle)\n"
"end\n"
"\n"
"function lxi_log_add(handle, ...)\n"
"\n"
"    local data = __lxi_log_data\n"
"\n"
"    i = #data[handle] + 1\n"
"\n"
"    data[handle][i] = {}\n"
"    for j,v in ipairs{...} do\n"
"        data[handle][i][j] = v\n"
"    end\n"
"end\n"
"\n"
"function lxi_log_save_csv(handle, filename)\n"
"\n"
"    local data = __lxi_log_data\n"
"\n"
"    file = io.open(filename, \"w\")\n"
"    io.output(file)\n"
"\n"
"    for i=1, #data[handle], 1\n"
"    do \n"
"        for j=1, #data[handle][i], 1\n"
"        do\n"
"            if (j == #data[handle][i])\n"
"            then\n"
"                io.write(data[handle][i][j])\n"
"            else\n"
"                io.write(data[handle][i][j] .. \",\")\n"
"            end\n"
"        end\n"
"        io.write(\"\\n\")\n"
"    end\n"
"\n"
"    io.close(file)\n"
"end\n";



// lua: device = lxi_connect(address, port, name, timeout, protocol)
static int connect(lua_State *L)
{
    int device;
    const char *address = lua_tostring(L, 1);
    int port = lua_tointeger(L, 2);
    const char *name = lua_tostring(L, 3);
    int timeout = lua_tointeger(L, 4);
    const char *protocol = lua_tostring(L, 5);

    // Default connect arguments
    int arg_port = 5025;
    const char *arg_name = "inst0";
    int arg_timeout = 2000;
    int arg_protocol = VXI11;

    // Handle port
    if (port != 0)
       arg_port = port;

    // Handle name
    if (name != NULL)
        arg_name = name;

    // Handle timeout
    if (timeout != 0)
       arg_timeout = timeout;

    // Handle protocol
    if (strcmp(protocol, "VXI11") == 0)
        arg_protocol = VXI11;
    if (strcmp(protocol, "RAW") == 0)
        arg_protocol = RAW;

    // Connect to LXI instrument using VXI11
    device = lxi_connect(address, arg_port, arg_name, arg_timeout, arg_protocol);
    if (device == LXI_ERROR)
        error_printf("Failed to connect\n");

    // Save session data for later reuse
    session[device].timeout = arg_timeout;
    session[device].protocol = arg_protocol;

    // Return status
    lua_pushinteger(L, device);
    return 1;
}

// lua: lxi_disconnect(device)
static int disconnect(lua_State *L)
{
    int status = 0;
    int device = lua_tointeger(L, 1);

    // Disconnect
    status = lxi_disconnect(device);

    // Return status
    lua_pushnumber(L, status);
    return 1;
}

// lua: scpi(device, command, timeout)
static int scpi(lua_State *L)
{
    char* response = malloc(RESPONSE_LENGTH_MAX);
    int status = 0, length;
    int device = lua_tointeger(L, 1);
    const char *command = lua_tostring(L, 2);
    int timeout = lua_tointeger(L, 3);
    char command_buffer[1000];

    // Use session timeout if no timeout provided
    if (timeout == 0)
        timeout = session[device].timeout;

    strip_trailing_space((char *) command);

    if (session[device].protocol == RAW)
    {
        // Add newline to command string
        strcpy(command_buffer, command);
        command_buffer[strlen(command)] = '\n';
        command_buffer[strlen(command)+1] = 0;
        command = command_buffer;
    }

    // Send SCPI command
    length = lxi_send(device, command, strlen(command), timeout);
    if (length < 0)
    {
        error_printf("Failed to send message\n");
        status = length;
        goto error;
    }

    // Only expect response in case we are firing a question command
    if (question(command))
    {
        length = lxi_receive(device, response, RESPONSE_LENGTH_MAX, timeout);
        if (length < 0)
        {
            error_printf("Failed to receive message\n");
            status = length;
            goto error;
        }
    }

    if (length > 0)
    {
        // Strip newline
        if (response[length-1] == '\n')
            response[--length] = 0;

        // Strip carriage return
        if (response[length-1] == '\r')
            response[--length] = 0;
    }

    lua_pushlstring(L, response, length);
    free(response);
    return 1;

error:
    // Return status
    lua_pushnumber(L, status);
    free(response);
    return 1;
}

// lua: scpi_raw(device, command, timeout)
static int scpi_raw(lua_State *L)
{
    char* response = malloc(RESPONSE_LENGTH_MAX);
    int status = 0, length;
    int device = lua_tointeger(L, 1);
    const char *command = lua_tostring(L, 2);
    int timeout = lua_tointeger(L, 3);

    // Use session timeout if no timeout provided
    if (timeout == 0)
        timeout = session[device].timeout;

    // Send SCPI command
    length = lxi_send(device, command, strlen(command), timeout);
    if (length < 0)
    {
        error_printf("Failed to send message\n");
        status = length;
        goto error;
    }

    // Only expect response in case we are firing a question command
    if (question(command))
    {
        length = lxi_receive(device, response, RESPONSE_LENGTH_MAX, timeout);
        if (length < 0)
        {
            error_printf("Failed to receive message\n");
            status = length;
            goto error;
        }
    }

    lua_pushlstring(L, response, length);
    free(response);
    return 1;

error:
    // Return status
    lua_pushnumber(L, status);
    free(response);
    return 1;
}


// lua: sleep(seconds)
static int sleep_(lua_State *L)
{
    long seconds = lua_tointeger(L, 1);

    sleep(seconds);

    return 0;
}

// lua: msleep(miliseconds)
static int msleep(lua_State *L)
{
    long mseconds = lua_tointeger(L, 1);
    long useconds = mseconds * 1000;

    usleep(useconds);

    return 0;
}

// lua: handle = clock_new()
static int clock_new(lua_State *L)
{
    int handle;

    // Find free clock
    for (handle=0; handle<CLOCKS_MAX; handle++)
    {
        if (lua_clock[handle].allocated == false)
        {
            lua_clock[handle].allocated = true;
            lua_clock[handle].time_start = 0;
            break;
        }
    }

    // Return clock handle
    lua_pushinteger(L, handle);
    return 1;
}

// lua: clock = clock_read()
static int clock_read(lua_State *L)
{
    int handle = lua_tointeger(L, 1);
    struct timespec time_spec;
    double time;

    // Get current time
    clock_gettime(CLOCK_MONOTONIC, &time_spec);
    time = time_spec.tv_sec;
    time += time_spec.tv_nsec * 0.000000001;

    // If first read call
    if (lua_clock[handle].time_start == 0)
    {
        // Save start time
        lua_clock[handle].time_start = time;

        // Return 0 time
        lua_pushnumber(L, 0);
        return 1;
    }

    // Return elapsed time since clock start
    time = time - lua_clock[handle].time_start;
    lua_pushnumber(L, time);
    return 1;
}

// lua: clock_reset()
static int clock_reset(lua_State *L)
{
    int handle = lua_tointeger(L, 1);

    lua_clock[handle].time_start = 0;

    return 0;
}

// lua: clock_free()
static int clock_free(lua_State *L)
{
    int handle = lua_tointeger(L, 1);

    lua_clock[handle].allocated = false;
    lua_clock[handle].time_start = 0;

    return 0;
}

static void load_log_script(lua_State *L)
{
    int error;

    error = luaL_loadbuffer(L, log_script, strlen(log_script), "lxi") ||
        lua_pcall(L, 0, 0, 0);
    if (error)
    {
        error_printf("%s\n", lua_tostring(L, -1));
        lua_pop(L, 1);  /* pop error message from the stack */
    }
}

static const struct luaL_Reg lxi_lib[] =
{
    { "lxi_connect", connect},
    { "lxi_disconnect", disconnect},
    { "lxi_scpi", scpi },
    { "lxi_scpi_raw", scpi_raw},
    { "lxi_sleep", sleep_},
    { "lxi_msleep", msleep},
    { "lxi_clock_new", clock_new},
    { "lxi_clock_read", clock_read},
    { "lxi_clock_reset", clock_reset},
    { "lxi_clock_free", clock_free},
    {NULL, NULL}
};

int lua_register_lxi(lua_State *L)
{
    // Register lxi functions
    lua_getglobal(L, "_G");
    luaL_setfuncs(L, lxi_lib, 0);
    lua_pop(L, 1);

    load_log_script(L);

    return 0;
}
