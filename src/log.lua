__lxi_log_data = {}

function lxi_log_new()

    local data = __lxi_log_data

    handle = #data + 1
    data[handle] = {}

    return handle
end

function lxi_log_free(handle)
--    table.remove(data, handle)
end

function lxi_log_add(handle, ...)

    local data = __lxi_log_data

    i = #data[handle] + 1

    data[handle][i] = {}
    for j,v in ipairs{...} do
        data[handle][i][j] = v
    end
end

function lxi_log_save_csv(handle, filename)

    local data = __lxi_log_data

    file = io.open(filename, "w")
    io.output(file)

    for i=1, #data[handle], 1
    do 
        for j=1, #data[handle][i], 1
        do
            if (j == #data[handle][i])
            then
                io.write(data[handle][i][j])
            else
                io.write(data[handle][i][j] .. ",")
            end
        end
        io.write("\n")
    end

    io.close(file)
end
