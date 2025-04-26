-------------------------------------
--  lxi-tools                      --
--    https://lxi-tools.github.io  --
-------------------------------------

-- Linear gauge chart test

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
chart0 = lxi_chart_new("linear-gauge",   -- chart type
                       "Engine",         -- title
                       "Throttle [ % ]", -- label
                       0, 100, 400)      -- value min, value max, window width

-- Manipulate gauge for 10 seconds
clock = 0
while (clock < 10)
do
   clock = lxi_clock_read(clock0)
   value = 50 + 20*math.sin(10*clock)
   lxi_chart_set_value(chart0, value)
   lxi_msleep(20)
end

-- Cleanup
lxi_clock_free(clock0)
lxi_chart_close(chart0)

print("Done")
