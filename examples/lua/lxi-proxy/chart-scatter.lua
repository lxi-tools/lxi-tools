-------------------------------------
--  lxi-tools                      --
--    https://lxi-tools.github.io  --
-------------------------------------

-- Scatter chart test - plotting a sine wave function

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
chart0 = lxi_chart_new("scatter",                              -- chart type
                       "Sine wave plot, f(t) = 5 + 2sin(10t)", -- title
                       "Time [ s ]",                           -- x-axis label
                       "Value [  ]",                           -- y-axis label
                       10, 10, 800)                            -- x max, y max, window width

-- Sample and plot sine wave at 100 Hz for 10 seconds
clock = 0
while (clock < 10)
do
   clock = lxi_clock_read(clock0)
   value = 5 + 2*math.sin(10*clock)
   lxi_chart_plot(chart0, clock, value)
   lxi_msleep(10)
end

-- Save chart data
lxi_chart_save_csv(chart0, dir_name.."/".."chart-scatter.csv")
lxi_chart_save_png(chart0, dir_name.."/".."chart-scatter.png")

-- Cleanup
lxi_clock_free(clock0)
lxi_chart_close(chart0)

print("Done")
