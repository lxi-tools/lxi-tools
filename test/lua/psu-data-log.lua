-------------------------------------
--  lxi-tools                      --
--    https://lxi-tools.github.io  --
-------------------------------------

-- Example: Data logging from a NGM202 PSU for 100 seconds at 10 Hz

-- Connect to power supply
psu = connect("192.168.0.107", nil, nil, 2000, "VXI11")
print("Power Supply ID = " .. scpi(psu,"*IDN?"))

-- Set power supply voltage on channel 1 to 5V
scpi(psu, "voltage 5.0, (@1)")

-- Turn on power supply
scpi(psu, "output on")

-- Wait for voltage to stabilize
msleep(1000)

-- Setup line chart and clock
chart0 = chart_new("line-chart",             -- chart type
                   "PSU Channel 1 Data Log", -- title
                   "Time [ s ]",             -- x-axis label
                   "Volt [ V ]",             -- y-axis label
                   100, 10, 800)             -- x max, y max, window width
clock0 = clock_new()

-- Capture and plot samples at 10 Hz for 100 seconds
clock = 0
while (clock < 100)
do
   voltage = scpi(psu, "voltage? (@1)")
   voltage = tonumber(voltage)
   clock = clock_read(clock0)
   chart_plot(chart0, clock, voltage)
   msleep(100)
end

-- Save data
chart_save_csv(chart, "chart0.csv")
chart_save_png(chart, "chart0.png")

-- Cleanup
clock_free(clock0)
chart_close(chart0)

-- Turn off power supply
scpi(psu, "output off")

-- Finish
print("Done")