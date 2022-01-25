-------------------------------------
--  lxi-tools                      --
--    https://lxi-tools.github.io  --
-------------------------------------

-- Line chart test - plotting a sine wave function

-- Init
clock0 = clock_new()
chart0 = chart_new("scatter-chart",                         -- chart type
                   "Sine wave plot, f(x) = 5 + 2sin(0.1x)", -- title
                   "Time [ s ]",                            -- x-axis label
                   "Value [  ]",                            -- y-axis label
                   10, 10, 800)                             -- x max, y max, window width

-- Sample and plot sine wave at 100 Hz for 10 seconds
clock = 0
x = 0
while (clock < 10)
do
   value = 5 + 2*math.sin(0.1*x)
   clock = clock_read(clock0)
   chart_plot(chart0, clock, value)
   x = x + 1
   msleep(10)
end

-- Cleanup
clock_free(clock0)
--chart_close(chart0)
print("Done")
