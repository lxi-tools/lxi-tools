-------------------------------------
--  lxi-tools                      --
--    https://lxi-tools.github.io  --
-------------------------------------

-- Linear gauge chart test

-- Init
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
