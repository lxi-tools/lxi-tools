-------------------------------------
--  lxi-tools                      --
--    https://lxi-tools.github.io  --
-------------------------------------

-- Number chart test

-- Init
clock0 = lxi_clock_new()
chart0 = lxi_chart_new("number",      -- chart type
                       "PSU Voltage", -- title
                       "[ V ]",       -- label
                       800)           -- window width

-- Manipulate number for 10 seconds
clock = 0
while (clock < 10)
do
   clock = lxi_clock_read(clock0)
   lxi_chart_set_value(chart0, clock)
   lxi_msleep(100)
end

-- Cleanup
lxi_clock_free(clock0)
lxi_chart_close(chart0)

print("Done")
