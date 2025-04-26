-------------------------------------
--  lxi-tools                      --
--    https://lxi-tools.github.io  --
-------------------------------------

-- Example: Retrieve ID from instrument

local lgi = require"lgi"
local Gio = lgi.Gio


local dir = Gio.File.new_for_commandline_arg(arg[0]):get_parent()
local dir_name = Gio.File.get_parse_name(dir);

package.path = dir_name..'/client/?.lua;' .. package.path
require "lxi-gui-api"


if (lxi_proxy_detect() ~= true) then
    print("lxi-proxy not started")
    return
end

local ip=lxi_selected_ip()

if(ip == nul) then
    print("No Instrument selected")
    return
end

device = lxi_connect_selected()

-- Print instrument ID
print("Device ID = " .. lxi_scpi(device,"*IDN?\n"))

-- Disconnect
lxi_disconnect_selected(device)

