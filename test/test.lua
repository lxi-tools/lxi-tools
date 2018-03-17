-- Connect to instruments
dso = connect("192.168.1.112")
psu = connect("192.168.1.144")

-- Print instrument IDs
dso_id = scpi(dso, "*IDN?")
psu_id = scpi(psu, "*IDN?")
print("Digital Storage Oscilloscope ID = " .. dso_id)
print("Power Supply ID = " .. psu_id)

-- Turn on power supply
scpi(psu, "output on");

-- Wait for voltage to stabilize
msleep(1000)

-- Read out power supply voltage
volt = scpi(psu, "voltage?")
volt = tonumber(volt)
print("volt = " .. volt)

-- Read out DSO voltage RMS
volt_rms = scpi(dso, ":MEASure:STATistic:ITEM? current,vrms")
volt_rms = tonumber(volt_rms)
print("volt_rms = " .. volt_rms)

-- Do voltage comparison
if (volt < volt_rms) then
    print("Power supply voltage is lower")
else
    print("Power supply voltage is higher")
end

-- Turn off power supply
scpi(psu, "output off");

-- Disconnect
disconnect(psu)
disconnect(dso)
