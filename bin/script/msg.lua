-- lua script

local pb = require("luapb")
pb.import("net.proto")

function tbcreate()
    local msg = pb.new("net.tb_Person")
    msg.number = "13615632545"
    msg.email = "13615632545@163.com"
    msg.age = 28
    msg.ptype = 2
    msg.desc:add("first")
    msg.desc:add("second")
    msg.desc:add("three")
end

function pbtest()
    local msg = pb.new("net.tb_Person")
    msg.number = "13615632545"
    msg.email = "13615632545@163.com"
    msg.age = 28
    msg.ptype = 2
    msg.desc:add("first")
    msg.desc:add("second")
    msg.desc:add("three")

    local binstr = pb.serializeToString(msg)
    local msg2 = pb.new("net.tb_Person")
    pb.parseFromString(msg2, binstr)

    print("pbtest " .. binstr)
end

function pbtest2()
    local msg = pb.new("net.tb_Person")
    msg.number = "13615632545"
    msg.email = "13615632545@163.com"
    msg.age = 28
    msg.ptype = 2
    msg.desc:add("first")
    msg.desc:add("second")
    msg.desc:add("three")

    for i = 1,1000000 do
        local binstr = pb.serializeToString(msg)
        local msg2 = pb.new("net.tb_Person")
        pb.parseFromString(msg2, binstr)
    end
end

function pbtest3()
    local msg = pb.new("net.tb_Person")
    msg.number = "luapb"
    msg.email = "luapb@163.com"
    msg.age2 = 28
    msg.ptype = 2
    msg.desc:add("first")
    msg.desc:add("second")
    msg.desc:add("three")

    local buffer = pb.encode("net.tb_Person", msg)
	local msg = pb.decode("net.tb_Person", buffer)
    local binstr = pb.serializeToString(msg)
    print("luapb pbtest " .. buffer)
    print("luapb binstr " .. binstr)  
end

function pbtest4()
    local msg = {}
    msg.number = "luaprotobuf"
    msg.email = "luaprotobuf@163.com"
    msg.age = 28
    msg.ptype = 2
    msg.desc = {"first", "second", "three"}

    for k, v in pairs(msg) do
        print(type(v))
        print(k)
    end

    --local buffer = pb.encode("net.tb_Person", msg)
	--local msg = pb.decode("net.tb_Person", buffer)

    --print("pbtest " .. buffer)
    --print("pbtest " .. msg.age)
end
pbtest()
pbtest3()