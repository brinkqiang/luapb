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

win
```
build.bat
cd bin\relwithdebinfo
lua ..\script\msg.lua  
```
linux
```
bash build.sh
cd bin/relwithdebinfo
./lua ../script/msg.lua  
```

```
pb_import enter
pb_import leave
pbtest 1361563254513615632545@163.com (2first2second2three
```

```lua
local pb = require("luapb")
local luajson = require("luajson")

pb.import("net.proto")

local name = pb.id2name(8345))
local id = pb.name2id("net.tb_Person"))

local msg = pb.pb_new("net.tb_Person")

msg.number = "13615632545"
msg.email = "13615632545@163.com"
msg.age = 28
msg.ptype = 2
msg.desc:add("first")
msg.desc:add("second")
msg.desc:add("three")

local msg_debugstring = pb.tostring(msg)

local buffer = pb.serializeToString(msg)

local msg2 = pb.new("net.tb_Person")
pb.parseFromString(msg2, buffer)

local tb_msg = {
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

local buffer2 = pb.encode("net.tb_Person", tb_msg)
local tb_msg2 = pb.decode("net.tb_Person", buffer2)

local str_json = pb.table2json("net.tb_Person", tb_msg2)
local str_xml = pb.table2xml("net.tb_Person", tb_msg2)
local str_yaml = pb.table2yaml("net.tb_Person", tb_msg2)

local tb_json = pb.json2table("net.tb_Person", str_json)
local tb_xml = pb.xml2table("net.tb_Person", str_xml)
local tb_yaml = pb.yaml2table("net.tb_Person", str_yaml)

```
## Contacts
[![Join the chat](https://badges.gitter.im/brinkqiang/luapb/Lobby.svg)](https://gitter.im/brinkqiang/luapb)

## Thanks
