-- lua script

local pb = require("luapb")
pb.import("net.proto")

function pb_table_create()
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
 
function pb_cpp_test()
    local msg = pb.new("net.tb_Person")
    msg.number = "13615632545"
    msg.email = "13615632545@163.com"
    msg.age = 28
    msg.ptype = 2
    msg.desc:add("first")
    msg.desc:add("second")
    msg.desc:add("three")

    msg.desc:set(1, "three")
    msg.desc:set(3, "first")
    for i = 1, msg.desc:len() do
        print("msg.desc[" .. i .. "] = " .. msg.desc:get(i))
    end

    local job = msg.jobs:add()
    job.jobtype = 8345
    job.jobdesc = "coder"

    local job2 = msg.jobs:add()
    job2.jobtype = 9527
    job2.jobdesc = "coder2"

    local buffer = pb.serializeToString(msg)
    local msg2 = pb.new("net.tb_Person")
    pb.parseFromString(msg2, buffer)

    print("pb_cpp_test pass #\n" ..  pb.tostring(msg2))
end

function pb_table_test() 
    local message = {
        number = "13615632545",
        email = "13615632545@163.com",
        age = 28,
        ptype = "WORK",
        desc = {"first", "second", "three"},
        jobs = {
            {
                jobtype = 8345,
                jobdesc = "coder"
            },
            {
                jobtype = 9527,
                jobdesc = "coder2"
            }
        }
    }
    for k, v in pairs(message) do
        print(type(v))
        print(k)
    end

    local buffer = pb.encode("net.tb_Person", message)
    local msg = pb.decode("net.tb_Person", buffer)

    local msg2 = pb.new("net.tb_Person")
    pb.parseFromString(msg2, buffer)

    assert(msg.number == "13615632545")
    assert(msg.email == "13615632545@163.com")
    assert(msg.age == 28)
    assert(msg.ptype == "WORK")
    assert(msg.desc[1] == "first")
    assert(msg.desc[2] == "second")
    assert(msg.desc[3] == "three")
    print("pb_table_test pass #\n" ..  pb.tostring(msg2))
end

function pb_cpp_test_million()
    for i=1, 1000000 do
        local msg = pb.new("net.tb_Person")
        msg.number = "13615632545"
        msg.email = "13615632545@163.com"
        msg.age = 28
        msg.ptype = 2
        msg.desc:add("first")
        msg.desc:add("second")
        msg.desc:add("three")

        local job = msg.jobs:add()
        job.jobtype = 8345
        job.jobdesc = "coder"
    
        local job2 = msg.jobs:add()
        job2.jobtype = 9527
        job2.jobdesc = "coder2"

        local buffer = pb.serializeToString(msg)
        local msg2 = pb.new("net.tb_Person")
        pb.parseFromString(msg2, buffer)
    end
    print("pb_cpp_test_million pass")
end

function pb_table_test_million() 
    for i=1, 1000000 do
        local message = {
            number = "13615632545",
            email = "13615632545@163.com",
            age = 28,
            ptype = "WORK",
            desc = {"first", "second", "three"},
            jobs = {
                {
                    jobtype = 8345,
                    jobdesc = "coder"
                },
                {
                    jobtype = 9527,
                    jobdesc = "coder2"
                }
            }
        }
   
        local buffer = pb.encode("net.tb_Person", message)
        local msg = pb.decode("net.tb_Person", buffer)
    end
    print("pb_table_test_million pass")
end

pb_cpp_test()
pb_table_test()

pb_cpp_test_million()
pb_table_test_million()