__log_data = {}

function log_new()

    local data = __log_data

    handle = #data + 1
    data[handle] = {}

    return handle
end

function log_free(handle)
--    table.remove(data, handle)
end

function log_add (handle, ...)

    local data = __log_data

    i = #data[handle] + 1

    data[handle][i] = {}
    for j,v in ipairs{...} do
        data[handle][i][j] = v
    end
end

function log_save_csv(handle, filename)

    local data = __log_data

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
