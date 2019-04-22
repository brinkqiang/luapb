# dmlua

Copyright (c) 2013-2018 brinkqiang (brink.qiang@gmail.com)

[![dmlua](https://img.shields.io/badge/brinkqiang-dmlua-blue.svg?style=flat-square)](https://github.com/brinkqiang/dmlua)
[![License](https://img.shields.io/badge/license-MIT-brightgreen.svg)](https://github.com/brinkqiang/dmlua/blob/master/LICENSE)
[![blog](https://img.shields.io/badge/Author-Blog-7AD6FD.svg)](https://brinkqiang.github.io/)
[![Open Source Love](https://badges.frapsoft.com/os/v3/open-source.png)](https://github.com/brinkqiang)
[![GitHub stars](https://img.shields.io/github/stars/brinkqiang/dmlua.svg?label=Stars)](https://github.com/brinkqiang/dmlua) 
[![GitHub forks](https://img.shields.io/github/forks/brinkqiang/dmlua.svg?label=Fork)](https://github.com/brinkqiang/dmlua)

## Build status
| [Linux][lin-link] | [MacOSX][osx-link] | [Windows][win-link] |
| :---------------: | :----------------: | :-----------------: |
| ![lin-badge]      | ![osx-badge]       | ![win-badge]        |

[lin-badge]: https://travis-ci.org/brinkqiang/dmlua.svg?branch=master "Travis build status"
[lin-link]:  https://travis-ci.org/brinkqiang/dmlua "Travis build status"
[osx-badge]: https://travis-ci.org/brinkqiang/dmlua.svg?branch=master "Travis build status"
[osx-link]:  https://travis-ci.org/brinkqiang/dmlua "Travis build status"
[win-badge]: https://ci.appveyor.com/api/projects/status/github/brinkqiang/dmlua?branch=master&svg=true "AppVeyor build status"
[win-link]:  https://ci.appveyor.com/project/brinkqiang/dmlua "AppVeyor build status"

## Intro
Lua fully automated engine, based on tolua++, support lua 5.3
```cpp
#include "test/role/rolemgr.h"
#include "test/role/role.h"
#include "dmlua.h"
#include "gtest/gtest.h"

TEST( luatest, luatest ) {
    CDMLuaEngine oDMLuaEngine;

    if ( !oDMLuaEngine.ReloadScript() ) {
        return;
    }

    oDMLuaEngine.DoString(
        "function addtest()\n"
        "   local a = 100000000\n"
        "   local b = 100000000\n"
        "   local c = a * b\n"
        "   print(c)\n"
        "end\n" );
    {
        for ( int i = 0; i < 1; ++i ) {
            int r = oDMLuaEngine.Call( "addtest" );
        }
    }

    if ( !oDMLuaEngine.ReloadScript() ) {
        return;
    }
    
    oDMLuaEngine.DoString(
        "function addex(first, second)\n"
        "   return first * second\n"
        "end\n" );
    {
        for ( int i = 0; i < 1; ++i ) {
            uint64_t r = oDMLuaEngine.CallT<uint64_t>("addex", 100000000LL, 100000000LL);

            if ( r >= 0 ) {
                std::cout << r << std::endl;
            }
        }
    }
    
    oDMLuaEngine.DoString(
        "function addex(first, second, res)\n"
        "   res.value = first * second\n"
        "end\n" );
    {
        LResultINT64 resAdd( -1 );

        for ( int i = 0; i < 1; ++i ) {
            int r = oDMLuaEngine.Call( "addex", 100000000LL, 100000000LL, &resAdd );

            if ( r >= 0 ) {
                std::cout << resAdd.value << std::endl;
            }
        }
    }
    {
        oDMLuaEngine.DoString(
            "function script.task.taskinfo(task)\n"
            "   task.nTaskID = 1002\n"
            "   task.nTaskState = 2\n"
            "   task.nTaskCondition = 2\n"
            "end\n" );
        STaskInfo sInfo;
        int r = oDMLuaEngine.Call( "script.task.taskinfo", &sInfo );

        if ( r >= 0 ) {
            std::cout << sInfo.nTaskID << std::endl;
        }
    }
    {
        oDMLuaEngine.DoString(
            "function script.task.taskinfo(task)\n"
            "   task.nTaskID = 1003\n"
            "   task.nTaskState = 1\n"
            "   task.nTaskCondition = 0\n"
            "end\n" );
        STaskInfo sInfo;
        int r = oDMLuaEngine.Call( "script.task.taskinfo", &sInfo );

        if ( r >= 0 ) {
            std::cout << sInfo.nTaskID << std::endl;
        }
    }
    CRole* poRole = CRoleMgr::Instance()->CreateRole();
    poRole->SetName( "andy" );
    poRole->SetHp( 9999 );
    poRole->SetMp( 9999 );
    unsigned int dwTaskID = 100;
    LResultINT oResult( -1 );
    oDMLuaEngine.Call( "script.task.task.AcceptTask", poRole, dwTaskID, &oResult );
    oDMLuaEngine.Call( "script.task.task.FinishTask", poRole, dwTaskID );
    std::vector<std::string> vecData;
    vecData.push_back( "hello" );
    oDMLuaEngine.Call( "script.common.test.main", &vecData );
    oDMLuaEngine.Call( "script.config.loadcsv.main" );
    CRoleMgr::Instance()->ReleaseRole( poRole );
}
```

```lua
-- lua script
module (..., package.seeall)

pb.import("net.proto")

function main()
  local msg = pb.new("net.tb_Person")
  msg.number = "13615632545"
  msg.email = "13615632545@163.com"
  msg.age = 28
  msg.ptype = 2
  msg.desc:add("first")
  msg.desc:add("second")
  msg.desc:add("three")
  
  local debugstr = pb.tostring(msg)
  local binstr = pb.serializeToString(msg)

  print("msgdebug:\n" .. debugstr)
  print("msgbin:\n" .. binstr)

  local msg2 = pb.new("net.tb_Person")
  pb.parseFromString(msg2, binstr)

  print("msg2debug:\n" .. pb.tostring(msg2))
  print("msg2bin:\n" .. pb.serializeToString(msg2))
end
```
## Contacts
[![Join the chat](https://badges.gitter.im/brinkqiang/dmlua/Lobby.svg)](https://gitter.im/brinkqiang/dmlua)

## Thanks
gavingqf@126.com
