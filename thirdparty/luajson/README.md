# luajson

Copyright (c) 2013-2018 brinkqiang (brink.qiang@gmail.com)

[![luajson](https://img.shields.io/badge/brinkqiang-luajson-blue.svg?style=flat-square)](https://github.com/brinkqiang/luajson)
[![License](https://img.shields.io/badge/license-MIT-brightgreen.svg)](https://github.com/brinkqiang/luajson/blob/master/LICENSE)
[![blog](https://img.shields.io/badge/Author-Blog-7AD6FD.svg)](https://brinkqiang.github.io/)
[![Open Source Love](https://badges.frapsoft.com/os/v3/open-source.png)](https://github.com/brinkqiang)
[![GitHub stars](https://img.shields.io/github/stars/brinkqiang/luajson.svg?label=Stars)](https://github.com/brinkqiang/luajson) 
[![GitHub forks](https://img.shields.io/github/forks/brinkqiang/luajson.svg?label=Fork)](https://github.com/brinkqiang/luajson)

## Build status
| [Linux][lin-link] | [MacOSX][osx-link] | [Windows][win-link] |
| :---------------: | :----------------: | :-----------------: |
| ![lin-badge]      | ![osx-badge]       | ![win-badge]        |

[lin-badge]: https://travis-ci.org/brinkqiang/luajson.svg?branch=master "Travis build status"
[lin-link]:  https://travis-ci.org/brinkqiang/luajson "Travis build status"
[osx-badge]: https://travis-ci.org/brinkqiang/luajson.svg?branch=master "Travis build status"
[osx-link]:  https://travis-ci.org/brinkqiang/luajson "Travis build status"
[win-badge]: https://ci.appveyor.com/api/projects/status/github/brinkqiang/luajson?branch=master&svg=true "AppVeyor build status"
[win-link]:  https://ci.appveyor.com/project/brinkqiang/luajson "AppVeyor build status"

## env
install cmake

## Intro
luajson
```cpp
local json = require('luajson')

local foo = json.decode([[
            {"name" : "lol", "age" : -1.5e+06, "foo" : ["bar", true, null]}
            ]])
foo.x = 'x'
-- foo[1] = 'hossa'
-- foo[2] = 'rossa'

print(foo.age)    -- -1500000
print(foo.name)   -- lol
print(foo.foo[1]) -- bar
print(foo.foo[2]) -- true
print(foo.foo[3]) -- null
-- print(json.isnull(foo.foo[3])) -- true
foo.foox = "omg :D"
foo.theNull = json.null
foo.itIs = true
foo.itIsNot = false
foo.isNil = nil
foo.a = 'a'

foo.subtable = {
	a = 'one',
	b = 'two'
}

local a = {
	test = 'hello',
	test2 = 'world'
}

a.myself = a

local str = json.encode(foo)
print(str)

local foo2 = json.decode(str)

print(json.encode(foo2)) -- {"name":"lol",age:-1500000,"foo":"omg :D"}
print(json.encode(nil))
print(json.encode {{a=1},{b=2}})
```
## Contacts
[![Join the chat](https://badges.gitter.im/brinkqiang/luajson/Lobby.svg)](https://gitter.im/brinkqiang/luajson)

## Thanks
