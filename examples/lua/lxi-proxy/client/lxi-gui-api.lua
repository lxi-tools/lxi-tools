local socket = require "socket"

local json = require "cjson"
local util = require "cjson.util"

local LXI_PROXY_PORT = 20040
local LXI_PROXY_ADDRESS="localhost"

local LXI_CONNECT=1
local LXI_SCPI=2
local LXI_DISCONNECT=3
local LXI_SLEEP=4
local LXI_MSSLEEP=5
local LXI_CLK_NEW=6
local LXI_CLK_READ=7
local LXI_CLK_RST=8
local LXI_CLK_FREE=9
local LXI_VERSION=10
local LXI_SELECTED_IP=11
local LXI_SELECTED_ID=12
local LXI_CHART_NEW=13
local LXI_CHART_PLOT=14
local LXI_CHART_VALUE=15
local LXI_CHART_SAVE_CSV=16
local LXI_CHART_SAVE_PNG=17
local LXI_CHART_CLOSE=18

local LXI_CONNECT_SELECTED=20
local LXI_DISCONNECT_SELECTED=21


local LXI_PROXY_DETECT=100

local udp = socket.udp()

udp:settimeout(10)
assert(udp:setsockname("*",0))
udp:setpeername(LXI_PROXY_ADDRESS, LXI_PROXY_PORT)


function lxi_proxy_detect()
    local cmd={}
    cmd.type = LXI_PROXY_DETECT
    local data_to_send = json.encode(cmd)
    udp:send(data_to_send)
    local rcv_data = udp:receive()
    if(rcv_data == nil) then
        return false
    end
    return true
end

function lxi_connect(address, port, name, timeout, protocol)
    local cmd={}
    cmd.type = LXI_CONNECT
    cmd.address = address
    cmd.port = port
    cmd.name = name
    cmd.timeout = timeout
    cmd.protocol = protocol
    local data_to_send = json.encode(cmd)
    udp:send(data_to_send)
    local rcv_data = udp:receive()
    if(rcv_data == nil) then
        return -100 - cmd.type
    end
    local ret = json.decode(rcv_data)

    return ret.device
end

function lxi_scpi(device, command)
    local cmd={}
    cmd.type = LXI_SCPI
    cmd.device = device
    cmd.command = command
    local data_to_send = json.encode(cmd)
    udp:send(data_to_send)
    local rcv_data = udp:receive()
    if(rcv_data == nil) then
        return -100 - cmd.type
    end
    local ret = json.decode(rcv_data)
    return ret.scpi_message

end

function lxi_disconnect(device)
    local cmd={}
    cmd.type = LXI_DISCONNECT
    cmd.device = device
    local data_to_send = json.encode(cmd)
    udp:send(data_to_send)
    local rcv_data = udp:receive()
    if(rcv_data == nil) then
        return -100 - cmd.type
    end
    local ret = json.decode(rcv_data)

end

function lxi_sleep(time)
    local cmd={}
    cmd.type = LXI_SLEEP
    cmd.time = time
    udp:settimeout(time + 1)
    local data_to_send = json.encode(cmd)
    udp:send(data_to_send)
    local rcv_data = udp:receive()
    if(rcv_data == nil) then
        return -100 - cmd.type
    end
    local ret = json.decode(rcv_data)
    udp:settimeout(10)
end

function lxi_msleep(time)
    local cmd={}
    cmd.type = LXI_MSSLEEP
    cmd.time = time
    udp:settimeout(time/1000 + 1)
    local data_to_send = json.encode(cmd)
    udp:send(data_to_send)
    local rcv_data = udp:receive()
    if(rcv_data == nil) then
        return -100 - cmd.type
    end
    local ret = json.decode(rcv_data)
    udp:settimeout(10)
end

function lxi_clock_new()
    local cmd={}
    cmd.type = LXI_CLK_NEW
    local data_to_send = json.encode(cmd)
    udp:send(data_to_send)
    local rcv_data = udp:receive()
    if(rcv_data == nil) then
        return -100 - cmd.type
    end
    local ret = json.decode(rcv_data)

    return ret.clock
end

function lxi_clock_read(clock)
    local cmd={}
    cmd.type = LXI_CLK_READ
    cmd.clock = clock
    local data_to_send = json.encode(cmd)
    udp:send(data_to_send)
    local rcv_data = udp:receive()
    if(rcv_data == nil) then
        return -100 - cmd.type
    end
    local ret = json.decode(rcv_data)
    return ret.clk_time
end

function lxi_clock_reset(clock)
    local cmd={}
    cmd.type = LXI_CLK_RST
    cmd.clock = clock
    local data_to_send = json.encode(cmd)
    udp:send(data_to_send)
    local rcv_data = udp:receive()
    if(rcv_data == nil) then
        return -100 - cmd.type
    end
    local ret = json.decode(rcv_data)

end

function lxi_clock_free(clock)
    local cmd={}
    cmd.type = LXI_CLK_FREE
    cmd.clock = clock
    local data_to_send = json.encode(cmd)
    udp:send(data_to_send)
    local rcv_data = udp:receive()
    if(rcv_data == nil) then
        return -100 - cmd.type
    end
    local ret = json.decode(rcv_data)

end

function lxi_version()
    local cmd={}
    cmd.type = LXI_VERSION
    local data_to_send = json.encode(cmd)
    udp:send(data_to_send)
    local rcv_data = udp:receive()
    if(rcv_data == nil) then
        return -100 - cmd.type
    end
    local ret = json.decode(rcv_data)

    return ret.version
end

function lxi_selected_ip()
    local cmd={}
    cmd.type = LXI_SELECTED_IP
    local data_to_send = json.encode(cmd)
    udp:send(data_to_send)
    local rcv_data = udp:receive()
    if(rcv_data == nil) then
        return -100 - cmd.type
    end
    local ret = json.decode(rcv_data)

    return ret.ip
end

function lxi_selected_id()
    local cmd={}
    cmd.type = LXI_SELECTED_ID
    local data_to_send = json.encode(cmd)
    udp:send(data_to_send)
    local rcv_data = udp:receive()
    if(rcv_data == nil) then
        return -100 - cmd.type
    end
    local ret = json.decode(rcv_data)

    return ret.id
end


function lxi_chart_new(chart_type, ...)
    local cmd={}
    cmd.type = LXI_CHART_NEW
    cmd.chart_type = chart_type
    cmd.args = {}
    for i = 1, select("#",...) do
        cmd.args[#cmd.args+1] = select(i,...)
    end

    local data_to_send = json.encode(cmd)
    udp:send(data_to_send)
    local rcv_data = udp:receive()
    if(rcv_data == nil) then
        return -100 - cmd.type
    end
    local ret = json.decode(rcv_data)

    return ret.chart
end

function lxi_chart_plot(chart, x, y)
    local cmd={}
    cmd.type = LXI_CHART_PLOT
    cmd.chart = chart
    cmd.x= x
    cmd.y = y
    local data_to_send = json.encode(cmd)
    udp:send(data_to_send)
    local rcv_data = udp:receive()
    if(rcv_data == nil) then
        return -100 - cmd.type
    end
    local ret = json.decode(rcv_data)
end

function lxi_chart_set_value(chart, value)
    local cmd={}
    cmd.type = LXI_CHART_VALUE
    cmd.chart = chart
    cmd.value= value
    local data_to_send = json.encode(cmd)
    udp:send(data_to_send)
    local rcv_data = udp:receive()
    if(rcv_data == nil) then
        return -100 - cmd.type
    end
    local ret = json.decode(rcv_data)

end

function lxi_chart_save_csv(chart, filename)
    local cmd={}
    cmd.type = LXI_CHART_SAVE_CSV
    cmd.filename = filename
    cmd.value= value
    local data_to_send = json.encode(cmd)
    udp:send(data_to_send)
    local rcv_data = udp:receive()
    if(rcv_data == nil) then
        return -100 - cmd.type
    end
    local ret = json.decode(rcv_data)

end

function lxi_chart_save_png(chart, filename)
    local cmd={}
    cmd.type = LXI_CHART_SAVE_PNG
    cmd.filename = filename
    cmd.value= value
    local data_to_send = json.encode(cmd)
    udp:send(data_to_send)
    local rcv_data = udp:receive()
    if(rcv_data == nil) then
        return -100 - cmd.type
    end
    local ret = json.decode(rcv_data)

end


function lxi_chart_close(chart)
    local cmd={}
    cmd.type = LXI_CHART_CLOSE
    cmd.chart = chart
    local data_to_send = json.encode(cmd)
    udp:send(data_to_send)
    local rcv_data = udp:receive()
    if(rcv_data == nil) then
        return -100 - cmd.type
    end
    local ret = json.decode(rcv_data)

end

function lxi_connect_selected()
    local cmd={}
    cmd.type = LXI_CONNECT_SELECTED
    local data_to_send = json.encode(cmd)
    udp:send(data_to_send)
    local rcv_data = udp:receive()
    if(rcv_data == nil) then
        return -100 - cmd.type
    end
    local ret = json.decode(rcv_data)
    return ret.device
end

function lxi_disconnect_selected(device)
    local cmd={}
    cmd.type = LXI_DISCONNECT_SELECTED
    cmd.device = device
    local data_to_send = json.encode(cmd)
    udp:send(data_to_send)
    local rcv_data = udp:receive()
    if(rcv_data == nil) then
        return -100 - cmd.type
    end
    local ret = json.decode(rcv_data)

end

--------------------------------------------------------
--             from lxi-tools source code             --
--------------------------------------------------------

local __lxi_log_data = {}

function lxi_log_new()

    local data = __lxi_log_data

    handle = #data + 1
    data[handle] = {}

    return handle
end

function lxi_log_free(handle)
--    table.remove(data, handle)
end

function lxi_log_add(handle, ...)

    local data = __lxi_log_data

    i = #data[handle] + 1

    data[handle][i] = {}
    for j,v in ipairs{...} do
        data[handle][i][j] = v
    end
end

function lxi_log_save_csv(handle, filename)

    local data = __lxi_log_data

    file = io.open(filename, "w")
    io.output(file)

    for i=1, #data[handle], 1
    do
        for j=1, #data[handle][i], 1
        do
            if (j == #data[handle][i])
            then
                io.write(data[handle][i][j])
            else
                io.write(data[handle][i][j] .. ",")
            end
        end
        io.write("\n")
    end

    io.close(file)
end
