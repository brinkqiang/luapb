# luapb

Copyright (c) 2013-2018 brinkqiang (brink.qiang@gmail.com)

[![luapb](https://img.shields.io/badge/brinkqiang-luapb-blue.svg?style=flat-square)](https://github.com/brinkqiang/luapb)
[![License](https://img.shields.io/badge/license-MIT-brightgreen.svg)](https://github.com/brinkqiang/luapb/blob/master/LICENSE)
[![blog](https://img.shields.io/badge/Author-Blog-7AD6FD.svg)](https://brinkqiang.github.io/)
[![Open Source Love](https://badges.frapsoft.com/os/v3/open-source.png)](https://github.com/brinkqiang)
[![GitHub stars](https://img.shields.io/github/stars/brinkqiang/luapb.svg?label=Stars)](https://github.com/brinkqiang/luapb) 
[![GitHub forks](https://img.shields.io/github/forks/brinkqiang/luapb.svg?label=Fork)](https://github.com/brinkqiang/luapb)

## Build status
| [Linux][lin-link] | [MacOSX][osx-link] | [Windows][win-link] |
| :---------------: | :----------------: | :-----------------: |
| ![lin-badge]      | ![osx-badge]       | ![win-badge]        |

[lin-badge]: https://travis-ci.org/brinkqiang/luapb.svg?branch=master "Travis build status"
[lin-link]:  https://travis-ci.org/brinkqiang/luapb "Travis build status"
[osx-badge]: https://travis-ci.org/brinkqiang/luapb.svg?branch=master "Travis build status"
[osx-link]:  https://travis-ci.org/brinkqiang/luapb "Travis build status"
[win-badge]: https://ci.appveyor.com/api/projects/status/github/brinkqiang/luapb?branch=master&svg=true "AppVeyor build status"
[win-link]:  https://ci.appveyor.com/project/brinkqiang/luapb "AppVeyor build status"

## Intro
luapb  
win lua ../script/msg.lua  
linux lua script/msg.lua
```
pb_import enter
pb_import leave
pbtest 1361563254513615632545@163.com (2first2second2three
```

```lua
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

pbtest()
```
## Contacts
[![Join the chat](https://badges.gitter.im/brinkqiang/luapb/Lobby.svg)](https://gitter.im/brinkqiang/luapb)

## Thanks
