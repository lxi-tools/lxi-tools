-------------------------------------
--  lxi-tools                      --
--    https://lxi-tools.github.io  --
-------------------------------------

-- Line chart test - plotting a sine wave function

-- Init
clock0 = clock_new()
chart0 = chart_new("line",                                 -- chart type
                   "Sine wave plot, f(t) = 5 + 2sin(10t)", -- title
                   "Time [ s ]",                           -- x-axis label
                   "Value [  ]",                           -- y-axis label
                   10, 10, 800)                            -- x max, y max, window width

-- Sample and plot sine wave at 100 Hz for 10 seconds
clock = 0
while (clock < 10)
do
   clock = clock_read(clock0)
   value = 5 + 2*math.sin(10*clock)
   chart_plot(chart0, clock, value)
   msleep(10)
end

-- Save chart data
chart_save_csv(chart0, "chart0.csv")
chart_save_png(chart0, "chart0.png")

-- Cleanup
clock_free(clock0)
chart_close(chart0)

print("Done")
