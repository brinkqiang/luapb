
// Copyright (c) 2018 brinkqiang (brink.qiang@gmail.com)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "luamsgconvert_module.h"
#include "sol.hpp"
#include "luamsgconvert.h"

namespace lua_module_luamsgconvert
{
    static sol::table require_api(sol::this_state L)
    {
        sol::state_view lua(L);
        sol::table module = lua.create_table();

        module.new_usertype<CLuaMsgConvert>("msg",
            sol::constructors<CLuaMsgConvert()>(),
            "Import", &CLuaMsgConvert::Import,
            "MsgConvert", &CLuaMsgConvert::MsgConvert
            );
        return module;
    }
}

LUAMOD_API int luaopen_luamsgconvert(lua_State* L)
{
    return sol::stack::call_lua(L, 1, lua_module_luamsgconvert::require_api);
}

LUAMOD_API int require_luamsgconvert(lua_State* L)
{
    luaL_requiref(L, "luamsgconvert", luaopen_luamsgconvert, 0);
    printf("lua module: require luamsgconvert\n");
    return 1;
}