cmake_minimum_required(VERSION 3.2)

PROJECT(luapb)
INCLUDE(cmake/ModuleImport.cmake)
INCLUDE(cmake/LuaModule.cmake)
LIST(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
INCLUDE(cmake/ModuleCompileOptions.cmake)
ModuleSetCompileOptions()
ModuleSetWinCompilerFlags()

ModuleImport("dmprotobuf" "thirdparty/dmprotobuf")
ModuleImport("dmyaml" "thirdparty/dmyaml")

LibImport("tolua" "src/tolua")

SET(LUA_MODULE lua53)
SET(LUAPB_VERSION_MAJOR "1")
SET(LUAPB_VERSION_MINOR "0")
SET(LUAPB_VERSION_PATCH "1")
SET(LUAPB_VERSION "${LUAPB_VERSION_MAJOR}.${LUAPB_VERSION_MINOR}.${LUAPB_VERSION_PATCH}")

MESSAGE(STATUS "VERSION: ${LUAPB_VERSION}")

ADD_DEFINITIONS(-DUSE_LUAPB)
ADD_DEFINITIONS(-DLUA_COMPAT_MODULE -DLUA_COMPAT_APIINTCASTS)

option(USE_INTERNAL_FPCONV "Use internal strtod() / g_fmt() code for performance" ON)
option(MULTIPLE_THREADS "Support multi-threaded apps with internal fpconv - recommended" ON)

IF(MSVC)
    ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS)
    ADD_DEFINITIONS(-Dstrncasecmp=_strnicmp)
ENDIF()

if(USE_INTERNAL_FPCONV)
    # Use libc number conversion routines (strtod(), sprintf())
    add_definitions(-DUSE_INTERNAL_FPCONV)
endif()

LuaModuleImport("${LUA_MODULE}" "luapb" "src/luapb" "tolua;dmyaml;protobuf::libprotobuf;protobuf::libprotoc")

LuaModuleImport("${LUA_MODULE}" "luajson" "src/luajson" "")
