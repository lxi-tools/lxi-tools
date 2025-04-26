

local lgi = require"lgi"
local Gio = lgi.Gio


local dir = Gio.File.new_for_commandline_arg(arg[0]):get_parent()
local dir_name = Gio.File.get_parse_name(dir);

package.path = dir_name..'/client/?.lua;' .. package.path
require "lxi-gui-api"


print(select(1, "a", "b", "c"))
print(select("#", {1,2,3}, 4, 5, {6,7,8}))
print(select(1, {1,2,3}, 4, 5, {6,7,8}))
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

