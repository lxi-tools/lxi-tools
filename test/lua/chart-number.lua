-------------------------------------
--  lxi-tools                      --
--    https://lxi-tools.github.io  --
-------------------------------------

-- Number chart test

-- Init
clock0 = clock_new()
chart0 = chart_new("number",      -- chart type
                   "PSU Voltage", -- title
                   "[ V ]",       -- label
                   800)           -- window width

-- Manipulate number for 10 seconds
clock = 0
while (clock < 10)
do
   clock = clock_read(clock0)
   chart_set_value(chart0, clock)
   msleep(100)
end

-- Cleanup
clock_free(clock0)
--chart_close(chart0)
print("Done")
