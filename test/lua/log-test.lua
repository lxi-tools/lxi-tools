-------------------------------------
--  lxi-tools                      --
--    https://lxi-tools.github.io  --
-------------------------------------

-- Data logger test - logging data and writing it to CSV file

-- Init
log0 = log_new()

-- Log some data
log_add(log0, 12, "text", 34, "text")
log_add(log0, 56, "text", "text", "text", 78)
log_add(log0, "text", 12, "text", 345, "text", 6)

-- Dump data to CSV file
log_write_csv(log0, "log0.csv")

-- Cleanup
log_free(log0)
print("Done")