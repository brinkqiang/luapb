-- lua script

local pb = require("luapb")
local luajson = require("luajson")
pb.import("net.proto")

function pb_msgconvert()
    local msg = {
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
    pb.msgconvert(10001, 10002, "net.tb_Person", msg);
end

function pb_msgconvert2()
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

    pb.msgconvert2(10001, 10002, msg);
end
pb_msgconvert()
pb_msgconvert2()