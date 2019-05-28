-- lua script

local pb = require("luapb")
pb.import("net.pb3.proto")

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

pbtest()

local message = {
    number = "13615632545",
    email = "13615632545@163.com",
    age = 28,
    ptype = "WORK",
    desc = {"first", "second", "three"}
}

function pbtest3()
    local message = {
        number = "13615632545",
        email = "13615632545@163.com",
        age = 28,
        ptype = "WORK",
        desc = {"first", "second", "three"}
    }
    for k, v in pairs(message) do
        print(type(v))
        print(k)
    end

    local buffer = pb.encode("net.tb_Person", message)
    local msg = pb.decode("net.tb_Person", buffer)

    assert(msg.number == "13615632545")
    assert(msg.email == "13615632545@163.com")
    assert(msg.age == 28)
    assert(msg.ptype == "WORK")
    assert(msg.desc[1] == "first")
    assert(msg.desc[2] == "second")
    assert(msg.desc[3] == "three")

    print("pbtest3 " .. buffer)
end

pbtest3()
