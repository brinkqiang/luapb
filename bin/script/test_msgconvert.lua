-- lua script

local pb = require("luapb")
pb.import("net.proto")

print("ID2Name: " .. pb.id2name(8000))
print("Name2ID: " .. pb.name2id("net.tb_Person"))

function bin2hex(s)
    s = string.gsub(s,"(.)",function (x) return string.format("%02X ",string.byte(x)) end)
    return s
end

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
    local sessionid = 100001
    local cmd = 10002
    local buf = pb.msgconvert_starve2comm(sessionid, cmd, "net.tb_Person", msg)
    print("msgconvert_starve2comm:   " .. sessionid .. "::" .. cmd .. "::\n" .. bin2hex(buf))

    local sessionid2, cmd2, msg2  = pb.msgconvert_comm2starve(buf)
    print("msgconvert_comm2starve:   " .. sessionid2 .. "::" .. cmd2 .. "::\n")

    local buf2 = pb.msgconvert_starve2comm(sessionid, cmd, "net.tb_Person", msg2)
    print("msgconvert_starve2comm:   " .. sessionid .. "::" .. cmd .. "::\n" .. bin2hex(buf2))
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

    local buf = pb.msgconvert2(10001, 10002, msg)
    print("pb_msgconvert2:  " .. bin2hex(buf))
end
pb_msgconvert()

print(pb.getmodulepath())
