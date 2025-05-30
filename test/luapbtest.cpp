
#include "luapb_module.h"
#include "sol/sol.hpp"
int main( int argc, char* argv[] ) {

    sol::state lua;
    lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::string, sol::lib::math, sol::lib::table, sol::lib::os, sol::lib::debug);
    sol::table luapb_module = lua.require("luapb", &require_luapb, true);

    return 0;
}
