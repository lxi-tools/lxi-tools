-------------------------------------
--  lxi-tools                      --
--    https://lxi-tools.github.io  --
-------------------------------------

-- Example: Working with multiple devices

-- Connect to instruments
dso = lxi_connect("192.168.0.157", nil, nil, 6000, "VXI11") -- R&S RTB2004
psu = lxi_connect("192.168.0.107", 5025, nil, 2000, "RAW") -- R&S NGM202

-- Print instrument IDs
print("Digital Storage Oscilloscope ID = " .. lxi_scpi(dso,"*IDN?"))
print("Power Supply ID = " .. lxi_scpi(psu,"*IDN?"))

-- Set power supply voltage on channel 1 to 1.2V
lxi_scpi(psu, "voltage 1.2, (@1)")

-- Turn on power supply
lxi_scpi(psu, "output on")

-- Wait for voltage to stabilize
lxi_msleep(1000)

-- Read out power supply voltage
volt_psu = lxi_scpi(psu, "voltage? (@1)")
volt_psu = tonumber(volt_psu)
print("volt_psu = " .. volt_psu)

-- Autoset oscilloscope
lxi_scpi(dso, "autoscale")

-- Start measurement on channel 1
lxi_scpi(dso, ":measurement1 on")
lxi_scpi(dso, ":measurement1:main rms")

-- Measure for 4 seconds
lxi_msleep(4000)

-- Read out DSO voltage of channel 1
volt_dso = lxi_scpi(dso, ":measurement1:result? rms")
volt_dso = tonumber(volt_dso)
print("volt_dso = " .. volt_dso)

-- Do voltage comparison
if (volt_psu < volt_dso) then
    print("Power supply voltage is lower")
else
    print("Power supply voltage is higher")
end

-- Turn off power supply
lxi_scpi(psu, "output off")

-- Disconnect
lxi_disconnect(psu)
lxi_disconnect(dso)

print("Done")
