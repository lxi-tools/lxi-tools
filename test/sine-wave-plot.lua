-------------------------------------
--  lxi-tools                      --
--    https://lxi-tools.github.io  --
-------------------------------------

-- Sine wave plot

-- Init
clock0 = clock_new()
chart0 = chart_new("line-chart",                         -- chart type
                   "Sine wave plot, f(x) = 5 + 2sin(x)", -- title
                   "Time [s]",                           -- x-axis title
                   "Value [ ]",                          -- y-axis title
                   10, 10, 1000)                         -- x max, y max, window width

-- Sample and plot sine wave at 100 Hz for 10 seconds
clock = 0
x = 0
while (clock < 10)
do
   value = 5 + 2*math.sin(x)
   clock = clock_read(clock0)
   chart_plot(chart0, clock, value)
   x = x + 1
   msleep(10)
end

-- Cleanup
chart_free(chart0)
clock_free(c0)
print("Done")
