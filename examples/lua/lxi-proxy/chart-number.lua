-------------------------------------
--  lxi-tools                      --
--    https://lxi-tools.github.io  --
-------------------------------------

-- Number chart test

-- Init
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


clock0 = lxi_clock_new()
chart0 = lxi_chart_new("number",      -- chart type
                       "PSU Voltage", -- title
                       "[ V ]",       -- label
                       800)           -- window width

-- Manipulate number for 10 seconds
clock = 0
while (clock < 10)
do
   clock = lxi_clock_read(clock0)
   lxi_chart_set_value(chart0, clock)
   lxi_msleep(100)
end

-- Cleanup
lxi_clock_free(clock0)
lxi_chart_close(chart0)

print("Done")
