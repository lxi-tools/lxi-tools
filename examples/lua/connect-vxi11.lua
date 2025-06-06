-------------------------------------
--  lxi-tools                      --
--    https://lxi-tools.github.io  --
-------------------------------------

-- Example: Retrieve ID from instrument (R&S NGM 202) via VXI11 protocol

-- Connect to power supply
psu = lxi_connect("192.168.0.107", nil, nil, 2000, "VXI11")

-- Print instrument ID
print("Power Supply ID = " .. lxi_scpi(psu,"*IDN?"))

-- Disconnect
lxi_disconnect(psu)

