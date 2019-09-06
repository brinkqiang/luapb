
local pb = require "luapb"
pb.import("test_luaprotobuf.proto")

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
local msg = pb.new("net.tb_Person")
pb.parseFromString(msg, chunk)
local buffer = pb.serializeToString(msg)

local chunk = pb.encode("Person",data)
--print(string.len(chunk))
local de = pb.decode("Person",chunk)
for k,v in pairs(de) do 
    print(k,v)
end 

function testn(num,f,...)
    local t1 = os.clock();
    for i=1,num do
        f(...)
    end 
    print("num="..num.."\ttime="..os.clock()-t1)
end 

testn(1000000,pb.serializeToString,"Person",msg)
testn(1000000,pb.parseFromString,"Person",buffer)

--testn(1000000,pb.encode,"Person",data)
--testn(1000000,pb.decode,"Person",chunk)

