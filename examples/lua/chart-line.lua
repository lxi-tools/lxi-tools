-------------------------------------
--  lxi-tools                      --
--    https://lxi-tools.github.io  --
-------------------------------------

-- Line chart test - plotting a sine wave function

-- Init
clock0 = lxi_clock_new()
chart0 = lxi_chart_new("line",                                -- chart type
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
lxi_chart_save_csv(chart0, "chart0.csv")
lxi_chart_save_png(chart0, "chart0.png")

-- Cleanup
lxi_clock_free(clock0)
--lxi_chart_close(chart0)

print("Done")
