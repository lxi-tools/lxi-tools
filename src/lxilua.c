/*
 * Copyright (c) 2018, Martin Lund
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
#include <lxi.h>
#include "options.h"
#include "error.h"

#define RESPONSE_LENGTH_MAX 0x400000

extern int question(const char *string);

// lua: device = lxi_connect(address)
static int connect(lua_State *L)
{
    int device;
    const char *address = lua_tostring(L, 1);

    // Connect to LXI instrument using VXI11
    device = lxi_connect(address, 0, NULL, option.timeout, VXI11);
    if (device == LXI_ERROR)
        error_printf("Failed to connect\n");

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

// lua: scpi(device, command)
static int scpi(lua_State *L)
{
    char response[RESPONSE_LENGTH_MAX];
    int status = 0, length;
    int device = lua_tointeger(L, 1);
    const char *command = lua_tostring(L, 2);

    // Send SCPI command
    length = lxi_send(device, command, strlen(command), option.timeout);
    if (length < 0)
    {
        error_printf("Failed to send message\n");
        status = length;
        goto error;
    }

    // Only expect response in case we are firing a question command
    if (question(command))
    {
        length = lxi_receive(device, response, RESPONSE_LENGTH_MAX, option.timeout);
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
    return 1;

error:
    // Return status
    lua_pushnumber(L, status);
    return 1;
}

// lua: scpi_raw(device, command)
static int scpi_raw(lua_State *L)
{
    char response[RESPONSE_LENGTH_MAX];
    int status = 0, length;
    int device = lua_tointeger(L, 1);
    const char *command = lua_tostring(L, 2);

    // Send SCPI command
    length = lxi_send(device, command, strlen(command), option.timeout);
    if (length < 0)
    {
        error_printf("Failed to send message\n");
        status = length;
        goto error;
    }

    // Only expect response in case we are firing a question command
    if (question(command))
    {
        length = lxi_receive(device, response, RESPONSE_LENGTH_MAX, option.timeout);
        if (length < 0)
        {
            error_printf("Failed to receive message\n");
            status = length;
            goto error;
        }
    }

    lua_pushlstring(L, response, length);
    return 1;

error:
    // Return status
    lua_pushnumber(L, status);
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

int luaopen_lxilua(lua_State *L)
{
    lua_register(L, "connect", connect);
    lua_register(L, "disconnect", disconnect);
    lua_register(L, "scpi", scpi);
    lua_register(L, "scpi_raw", scpi_raw);
    lua_register(L, "sleep", sleep_);
    lua_register(L, "msleep", msleep);
    return 0;
}
