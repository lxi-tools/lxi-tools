-------------------------------
-- Test script for lxi-tools --
--                           --
-- To run it simply do:      --
--                           --
-- $ lxi run test.lua        --
-------------------------------

-- Connect to instruments
dso = connect("192.168.0.157", 5025, nil, 6000, "VXI11")
psu = connect("192.168.0.107", 5025, nil, 2000, "RAW")

-- Print instrument IDs
dso_id = scpi(dso, "*IDN?")
psu_id = scpi(psu, "*IDN?")
print("Digital Storage Oscilloscope ID = " .. dso_id)
print("Power Supply ID = " .. psu_id)

-- Set power supply voltage on channel 1 to 1.2V
scpi(psu, "voltage 1.2, (@1)")

-- Turn on power supply
scpi(psu, "output on")

-- Wait for voltage to stabilize
msleep(1000)

-- Read out power supply voltage
volt = scpi(psu, "voltage? (@1)")
volt = tonumber(volt)
print("volt = " .. volt)

-- Autoset oscilloscope
scpi(dso, "autoscale")

-- Start measurement on channel 1
scpi(dso, ":measurement1 on")
scpi(dso, ":measurement1:main rms")

-- Measure for a bit
msleep(4000)

-- Read out DSO voltage of channel 1
volt_rms = scpi(dso, ":measurement1:result? rms")
volt_rms = tonumber(volt_rms)
print("volt_rms = " .. volt_rms)

-- Do voltage comparison
if (volt < volt_rms) then
    print("Power supply voltage is lower")
else
    print("Power supply voltage is higher")
end

-- Turn off power supply
scpi(psu, "output off")

-- Disconnect
disconnect(psu)
disconnect(dso)
