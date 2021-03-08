cmake_minimum_required(VERSION 3.2)

PROJECT(luajson)
INCLUDE(cmake/ModuleImport.cmake)
INCLUDE(cmake/LuaModule.cmake)
LIST(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
INCLUDE(cmake/ModuleCompileOptions.cmake)
ModuleSetCompileOptions()
ModuleSetWinCompilerFlags()

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)

SET(LUA_MODULE lua53)
SET(LUAJSON_VERSION_MAJOR "1")
SET(LUAJSON_VERSION_MINOR "0")
SET(LUAJSON_VERSION_PATCH "1")
SET(LUAJSON_VERSION "${LUAJSON_VERSION_MAJOR}.${LUAJSON_VERSION_MINOR}.${LUAJSON_VERSION_PATCH}")

MESSAGE(STATUS "VERSION: ${LUAJSON_VERSION}")

ADD_DEFINITIONS(-DUSE_LUAJSON)
ADD_DEFINITIONS(-DLUA_COMPAT_MODULE -DLUA_COMPAT_APIINTCASTS)

option(USE_INTERNAL_FPCONV "Use internal strtod() / g_fmt() code for performance" ON)
option(MULTIPLE_THREADS "Support multi-threaded apps with internal fpconv - recommended" ON)

IF(MSVC)
    ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS)
    ADD_DEFINITIONS(-Dinline=__inline)
    #ADD_DEFINITIONS(-Dsnprintf=_snprintf)
    ADD_DEFINITIONS(-Dstrncasecmp=_strnicmp)
ENDIF()

if(USE_INTERNAL_FPCONV)
    # Use libc number conversion routines (strtod(), sprintf())
    add_definitions(-DUSE_INTERNAL_FPCONV)
endif()

LuaModuleImport("${LUA_MODULE}" "luajson" "src/luajson" "")

