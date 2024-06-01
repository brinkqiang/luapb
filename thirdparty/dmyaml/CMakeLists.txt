cmake_minimum_required(VERSION 3.21)

PROJECT(dmyaml)

LIST(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
INCLUDE(cmake/ModuleImport.cmake)
INCLUDE(cmake/ModuleCompileOptions.cmake)
ModuleSetCompileOptions()

SET(DMYAML_VERSION_MAJOR "1")
SET(DMYAML_VERSION_MINOR "0")
SET(DMYAML_VERSION_PATCH "1")
SET(DMYAML_VERSION "${DMYAML_VERSION_MAJOR}.${DMYAML_VERSION_MINOR}.${DMYAML_VERSION_PATCH}")

MESSAGE(STATUS "VERSION: ${DMYAML_VERSION}")

OPTION(USE_DMYAML "use dmyaml" OFF)
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/include/dmyaml_config.h.in ${CMAKE_CURRENT_SOURCE_DIR}/include/dmyaml_config.h)

ModuleInclude("yaml-cpp" "")
LibImport("dmyaml" "src")
