local protoc = require "protoc"
local pb = require "pb"
local buffer = require "pb.buffer"
protoc:new():loadfile("test.proto")

function test_encode(num)
    local de = pb.decode("Person", chunk)
    local data = {
        contacts = {{phonenumber = 123456789, name = "foo1"}},
        name = "hello",
        age = 18,
        address = "12345你好啊"
    }
    local loop = 80
    for i = 1, loop do
        table.insert(data.contacts,
                     {phonenumber = 123456789, name = "foo" .. (i + 1)})
    end
    local b = buffer.new()
    local chunk = pb.encode("Person", data, b)
    -- print(string.len(chunk))
    local de = pb.decode("Person", chunk)
    local t1 = os.clock()
    for i = 1, num do local msg = pb.encode("Person", data) end
    print("num=" .. num .. "\ttime=" .. os.clock() - t1)
end

function test_decode(num)
    local data = {
        contacts = {{phonenumber = 123456789, name = "foo1"}},
        name = "hello",
        age = 18,
        address = "12345你好啊"
    }
    local loop = 80
    for i = 1, loop do
        table.insert(data.contacts,
                     {phonenumber = 123456789, name = "foo" .. (i + 1)})
    end
    local b = buffer.new()
    local chunk = pb.encode("Person", data, b)
    -- print(string.len(chunk))
    local de = pb.decode("Person", chunk)

    local t1 = os.clock()
    for i = 1, num do local buf = pb.decode("Person", chunk) end
    print("num=" .. num .. "\ttime=" .. os.clock() - t1)
end

test_encode(1000000)

-- print(#chunk)
test_decode(1000000)

