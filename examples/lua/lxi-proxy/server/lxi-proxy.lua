local socket = require "socket"
local json = require "cjson"


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
udp:settimeout(1) -- set timeout to be able to stop script
udp:setsockname('*', LXI_PROXY_PORT)

print("lxi-tools proxy started on port ".. LXI_PROXY_PORT)

while (true) do
    local data, msg_or_ip, port_or_nil = udp:receivefrom()
    if data then
	local cmd = json.decode(data)

        if(cmd.type == LXI_PROXY_DETECT) then
            local ret={}
            ret.type = type_cmd
            local data_to_send = json.encode(ret)
            udp:sendto(data_to_send, msg_or_ip, port_or_nil)
        end
        if(cmd.type == LXI_CONNECT) then
            local type_cmd = cmd.type
            local address = cmd.address
            local port = cmd.port
            local name = cmd.name
            local timeout = cmd.timeout
            local protocol = cmd.protocol
            local device = lxi_connect(address, port, name, timeout, protocol)
            local ret={}
            ret.type = type_cmd
            ret.device=device
            local data_to_send = json.encode(ret)
            udp:sendto(data_to_send, msg_or_ip, port_or_nil)
        end
        if(cmd.type == LXI_SCPI) then
            local type_cmd = cmd.type
            local command = cmd.command
            local device = cmd.device
            local scpi_message = lxi_scpi(device, command)
            local ret={}
            ret.type = type_cmd
            ret.scpi_message = scpi_message
            local data_to_send = json.encode(ret)
            udp:sendto(data_to_send, msg_or_ip, port_or_nil)
        end
        if(cmd.type == LXI_DISCONNECT) then
            local type_cmd = cmd.type
            local device = cmd.device
            lxi_disconnect(device)
            local ret={}
            ret.type = type_cmd
            local data_to_send = json.encode(ret)
            udp:sendto(data_to_send, msg_or_ip, port_or_nil)
        end
        if(cmd.type == LXI_SLEEP) then
            local type_cmd = cmd.type
            local time = cmd.time
            lxi_sleep(time)
            local ret={}
            ret.type = type_cmd
            local data_to_send = json.encode(ret)
            udp:sendto(data_to_send, msg_or_ip, port_or_nil)
        end
        if(cmd.type == LXI_MSSLEEP) then
            local type_cmd = cmd.type
            local time = cmd.time
            lxi_msleep(time)
            local ret={}
            ret.type = type_cmd
            local data_to_send = json.encode(ret)
            udp:sendto(data_to_send, msg_or_ip, port_or_nil)
        end
        if(cmd.type == LXI_CLK_NEW) then
            local type_cmd = cmd.type
            local clock = lxi_clock_new()
            local ret={}
            ret.type = type_cmd
            ret.clock = clock
            local data_to_send = json.encode(ret)
            udp:sendto(data_to_send, msg_or_ip, port_or_nil)
        end
        if(cmd.type == LXI_CLK_READ) then
            local type_cmd = cmd.type
            local clock= cmd.clock
            local clk_time = lxi_clock_read(clock)
            local ret={}
            ret.type = type_cmd
            ret.clk_time = clk_time
            local data_to_send = json.encode(ret)
            udp:sendto(data_to_send, msg_or_ip, port_or_nil)
        end
        if(cmd.type == LXI_CLK_RST) then
            local type_cmd = cmd.type
            local clock= cmd.clock
            lxi_clock_reset(clock)
            local ret={}
            ret.type = type_cmd
            local data_to_send = json.encode(ret)
            udp:sendto(data_to_send, msg_or_ip, port_or_nil)
        end
        if(cmd.type == LXI_CLK_FREE) then
            local type_cmd = cmd.type
            local clock= cmd.clock
            lxi_clock_free(clock)
            local ret={}
            ret.type = type_cmd
            local data_to_send = json.encode(ret)
            udp:sendto(data_to_send, msg_or_ip, port_or_nil)
        end
        if(cmd.type == LXI_VERSION) then
            local type_cmd = cmd.type
            local version = lxi_version()
            local ret={}
            ret.type = type_cmd
            ret.version = version
            local data_to_send = json.encode(ret)
            udp:sendto(data_to_send, msg_or_ip, port_or_nil)
        end
        if(cmd.type == LXI_SELECTED_IP) then
            local type_cmd = cmd.type
            local ip = lxi_selected_ip()
            local ret={}
            ret.type = type_cmd
            ret.ip = ip
            local data_to_send = json.encode(ret)
            udp:sendto(data_to_send, msg_or_ip, port_or_nil)
        end
        if(cmd.type == LXI_SELECTED_ID) then
            local type_cmd = cmd.type
            local id = lxi_selected_id()
            local ret={}
            ret.type = type_cmd
            ret.id = id
            local data_to_send = json.encode(ret)
            udp:sendto(data_to_send, msg_or_ip, port_or_nil)
        end
        if(cmd.type == LXI_CHART_NEW) then
            local type_cmd = cmd.type
            local chart_type = cmd.chart_type
            local args = cmd.args
            local chart = lxi_chart_new(chart_type, table.unpack(args))
            local ret={}
            ret.type = type_cmd
            ret.chart = chart
            local data_to_send = json.encode(ret)
            udp:sendto(data_to_send, msg_or_ip, port_or_nil)
        end
        if(cmd.type == LXI_CHART_PLOT) then
            local type_cmd = cmd.type
            local chart= cmd.chart
            local x= cmd.x
            local y= cmd.y
            lxi_chart_plot(chart, x, y)
            local ret={}
            ret.type = type_cmd
            local data_to_send = json.encode(ret)
            udp:sendto(data_to_send, msg_or_ip, port_or_nil)
        end
        if(cmd.type == LXI_CHART_VALUE) then
            local type_cmd = cmd.type
            local chart= cmd.chart
            local value= cmd.value
            lxi_chart_set_value(chart, value)
            local ret={}
            ret.type = type_cmd
            local data_to_send = json.encode(ret)
            udp:sendto(data_to_send, msg_or_ip, port_or_nil)
        end
        if(cmd.type == LXI_CHART_SAVE_CSV) then
            local type_cmd = cmd.type
            local chart= cmd.chart
            local filename= cmd.filename
            lxi_chart_save_csv(chart, filename)
            local ret={}
            ret.type = type_cmd
            local data_to_send = json.encode(ret)
            udp:sendto(data_to_send, msg_or_ip, port_or_nil)
        end
        if(cmd.type == LXI_CHART_SAVE_PNG) then
            local type_cmd = cmd.type
            local chart= cmd.chart
            local filename= cmd.filename
            lxi_chart_save_png(chart, filename)
            local ret={}
            ret.type = type_cmd
            local data_to_send = json.encode(ret)
            udp:sendto(data_to_send, msg_or_ip, port_or_nil)
        end
        if(cmd.type == LXI_CHART_CLOSE) then
            local type_cmd = cmd.type
            local chart= cmd.chart
            lxi_chart_close(chart)
            local ret={}
            ret.type = type_cmd
            local data_to_send = json.encode(ret)
            udp:sendto(data_to_send, msg_or_ip, port_or_nil)
        end
        if(cmd.type == LXI_CONNECT_SELECTED) then
            local type_cmd = cmd.type
            local device = lxi_connect_selected(address, port, name, timeout, protocol)
            local ret={}
            ret.type = type_cmd
            ret.device=device
            local data_to_send = json.encode(ret)
            udp:sendto(data_to_send, msg_or_ip, port_or_nil)
        end
        if(cmd.type == LXI_DISCONNECT_SELECTED) then
            local type_cmd = cmd.type
            local device = cmd.device
            lxi_disconnect_selected(device)
            local ret={}
            ret.type = type_cmd
            local data_to_send = json.encode(ret)
            udp:sendto(data_to_send, msg_or_ip, port_or_nil)
        end
    end
end
