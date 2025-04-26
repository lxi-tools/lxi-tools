-------------------------------------
--  lxi-tools                      --
--    https://lxi-tools.github.io  --
-------------------------------------

-- Example: Retrieve ID from instrument (R&S NGM 202) via VXI11 protocol

-- Connect to power supply

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

device = lxi_connect("192.168.10.239", nil, nil, 2000, "VXI11")

-- Print instrument ID
print("Device ID = " .. lxi_scpi(device,"*IDN?"))

-- Disconnect
lxi_disconnect(psu)

