
local pb = require "luapb"
pb.import("test_luaprotobuf.proto")

function test_luapb_encode(num)
    local data = {
        contacts = {
            {phonenumber = 123456789, name = "foo1"},
        } , 
        name = "hello",age=18,address="12345你好啊"
    }
    local loop = 80
    for i=1,loop do 
        table.insert(data.contacts,{phonenumber = 123456789, name = "foo"..(i+1)})
    end 
    
    local chunk = pb.encode("Person",data)
    local msg = pb.new("Person")
    pb.parseFromString(msg, chunk)
    local buffer = pb.serializeToString(msg)

    local t1 = os.clock();
    for i=1,num do
        local buf = pb.serializeToString(msg)
    end 
    print("num=".. num .."\ttime="..os.clock()-t1)
end 

function test_luapb_decode(num)
    local data = {
        contacts = {
            {phonenumber = 123456789, name = "foo1"},
        } , 
        name = "hello",age=18,address="12345你好啊"
    }
    local loop = 80
    for i=1,loop do 
        table.insert(data.contacts,{phonenumber = 123456789, name = "foo"..(i+1)})
    end 
    
    local chunk = pb.encode("Person", data)
    local msg = pb.new("Person")
    pb.parseFromString(msg, chunk)
    local buffer = pb.serializeToString(msg)

    local t1 = os.clock();
    for i=1,num do
        local buf = pb.parseFromString(msg)
    end 
    print("num=".. num .."\ttime="..os.clock()-t1)
end 

test_luapb_encode(1000000)
test_luapb_decode(1000000)

--testn(1000000,pb.encode,"Person",data)
--testn(1000000,pb.decode,"Person",chunk)

