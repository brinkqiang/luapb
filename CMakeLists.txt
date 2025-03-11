cmake_minimum_required(VERSION 3.21)

project(luapb)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(cmake/ModuleImport.cmake)
include(cmake/LuaModule.cmake)
include(cmake/ModuleCompileOptions.cmake)
ModuleSetCompileOptions()
ModuleSetWinCompilerFlags()

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

ModuleImport("dmprotobuf" "thirdparty/dmprotobuf")
ModuleImport("dmyaml" "thirdparty/dmyaml")

LibImport("tolua" "src/tolua")

set(LUA_MODULE lua54)
set(LUAPB_VERSION_MAJOR "1")
set(LUAPB_VERSION_MINOR "1")
set(LUAPB_VERSION_PATCH "0")
set(LUAPB_VERSION "${LUAPB_VERSION_MAJOR}.${LUAPB_VERSION_MINOR}.${LUAPB_VERSION_PATCH}")

message(STATUS "VERSION: ${LUAPB_VERSION}")

add_definitions(-DUSE_LUAPB)
add_definitions(-DLUA_COMPAT_MODULE -DLUA_COMPAT_APIINTCASTS)

option(USE_INTERNAL_FPCONV "Use internal strtod() / g_fmt() code for performance" ON)
option(MULTIPLE_THREADS "Support multi-threaded apps with internal fpconv - recommended" ON)

IF(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    add_definitions(-Dstrncasecmp=_strnicmp)
ENDIF()

if(USE_INTERNAL_FPCONV)
    # Use libc number conversion routines (strtod(), sprintf())
    add_definitions(-DUSE_INTERNAL_FPCONV)
endif()

LuaModuleImport("${LUA_MODULE}" "luapb" "src/luapb" "tolua;dmyaml;protobuf::libprotobuf;protobuf::libprotoc")

LuaModuleImport("${LUA_MODULE}" "luajson" "src/luajson" "")
