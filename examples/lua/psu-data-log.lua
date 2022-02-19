-------------------------------------
--  lxi-tools                      --
--    https://lxi-tools.github.io  --
-------------------------------------

-- Example: Data logging from a NGM202 PSU for 25 seconds at 10 Hz

-- Connect to power supply
psu = connect("192.168.0.107", nil, nil, 2000, "VXI11")
print("Power Supply ID = " .. scpi(psu,"*IDN?"))

-- Set power supply voltage on channel 1 to 5V
scpi(psu, "voltage 5.0, (@1)")

-- Turn on power supply
scpi(psu, "output on")

-- Wait for power to stabilize
msleep(100)

-- Setup line chart and clock
chart0 = chart_new("line",                   -- chart type
                   "PSU Channel 1 Data Log", -- title
                   "Time [ s ]",             -- x-axis label
                   "Current [ I ]",          -- y-axis label
                   25, 1, 800)               -- x max, y max, window width
clock0 = clock_new()

-- Capture and plot samples at 10 Hz for 25 seconds
clock = 0
while (clock < 25)
do
   current = scpi(psu, "measure:current? (@1)")
   current = tonumber(current)
   clock = clock_read(clock0)
   chart_plot(chart0, clock, current)
   msleep(100)
end

-- Save data
chart_save_csv(chart0, "chart0.csv")
chart_save_png(chart0, "chart0.png")

-- Cleanup
clock_free(clock0)
chart_close(chart0)

-- Turn off power supply
scpi(psu, "output off")

-- Finish
print("Done")
