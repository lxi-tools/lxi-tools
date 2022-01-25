-------------------------------------
--  lxi-tools                      --
--    https://lxi-tools.github.io  --
-------------------------------------

-- Angular gauge chart test

-- Init
clock0 = clock_new()
chart0 = chart_new("angular-gauge",      -- chart type
                   "Engine",             -- title
                   "Tachometer [ RPM ]", -- label
                   0, 5000, 400)         -- value min, value max, window width

-- Manipulate gauge for 10 seconds
clock = 0
x = 0
while (clock < 10)
do
   value = 3000 + 1000*math.sin(0.1*x)
   clock = clock_read(clock0)
   chart_set_value(chart0, value)
   x = x + 1
   msleep(50)
end

-- Cleanup
clock_free(clock0)
--chart_close(chart0)
print("Done")
