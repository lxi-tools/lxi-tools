-------------------------------------
--  lxi-tools                      --
--    https://lxi-tools.github.io  --
-------------------------------------

-- Example: Data logging from a NGM202 PSU for 25 seconds at 10 Hz

-- Connect to power supply
psu = lxi_connect("192.168.0.107", nil, nil, 2000, "VXI11")
print("Power Supply ID = " .. lxi_scpi(psu,"*IDN?"))

-- Set power supply voltage on channel 1 to 5V
lxi_scpi(psu, "voltage 5.0, (@1)")

-- Turn on power supply
lxi_scpi(psu, "output on")

-- Wait for power to stabilize
lxi_msleep(100)

-- Setup line chart and clock
chart0 = lxi_chart_new("line",                   -- chart type
                       "PSU Channel 1 Data Log", -- title
                       "Time [ s ]",             -- x-axis label
                       "Current [ I ]",          -- y-axis label
                       25, 1, 800)               -- x max, y max, window width
clock0 = lxi_clock_new()

-- Capture and plot samples at 10 Hz for 25 seconds
clock = 0
while (clock < 25)
do
   current = lxi_scpi(psu, "measure:current? (@1)")
   current = tonumber(current)
   clock = lxi_clock_read(clock0)
   lxi_chart_plot(chart0, clock, current)
   lxi_msleep(100)
end

-- Save data
lxi_chart_save_csv(chart0, "chart0.csv")
lxi_chart_save_png(chart0, "chart0.png")

-- Cleanup
lxi_clock_free(clock0)
lxi_chart_close(chart0)

-- Turn off power supply
lxi_scpi(psu, "output off")

-- Finish
print("Done")
