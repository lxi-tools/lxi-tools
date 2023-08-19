-------------------------------------
--  lxi-tools                      --
--    https://lxi-tools.github.io  --
-------------------------------------

-- Example: Retrieve ID from instrument (R&H NGM 202) via RAW (socket)

-- Connect to power supply
psu = lxi_connect("192.168.0.107", 5025, nil, 2000, "RAW")

-- Print instrument ID
print("Power Supply ID = " .. lxi_scpi(psu,"*IDN?\n"))

-- Disconnect
lxi_disconnect(psu)

