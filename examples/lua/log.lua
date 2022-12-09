-------------------------------------
--  lxi-tools                      --
--    https://lxi-tools.github.io  --
-------------------------------------

-- Data logger test - logging data and writing it to CSV file

-- Init
log0 = lxi_log_new()

-- Log some data
lxi_log_add(log0, 12, "text", 34, "text")
lxi_log_add(log0, 56, "text", "text", "text", 78)
lxi_log_add(log0, "text", 12, "text", 345, "text", 6)

-- Dump data to CSV file
lxi_log_save_csv(log0, "log0.csv")

-- Cleanup
lxi_log_free(log0)

print("Done")
