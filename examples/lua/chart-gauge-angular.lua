-------------------------------------
--  lxi-tools                      --
--    https://lxi-tools.github.io  --
-------------------------------------

-- Angular gauge chart test

-- Init
clock0 = lxi_clock_new()
chart0 = lxi_chart_new("angular-gauge",      -- chart type
                       "Engine",             -- title
                       "Tachometer [ RPM ]", -- label
                       0, 5000, 400)         -- value min, value max, window width

-- Manipulate gauge for 10 seconds
clock = 0
while (clock < 10)
do
   value = 3000 + 1000*math.sin(clock)
   clock = lxi_clock_read(clock0)
   lxi_chart_set_value(chart0, value)
   lxi_msleep(50)
end

-- Cleanup
lxi_clock_free(clock0)
lxi_chart_close(chart0)

print("Done")
