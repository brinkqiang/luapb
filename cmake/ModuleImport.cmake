
macro(ModuleImport ModuleName ModulePath)

    MESSAGE(STATUS ${ModuleName})
    MESSAGE(STATUS ${ModulePath})

    LINK_DIRECTORIES(${CMAKE_SOURCE_DIR}/bin)
    SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)
    SET(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)

    IF (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${ModulePath}/CMakeLists.txt)
        ADD_SUBDIRECTORY(${ModulePath})
    ELSEIF(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${ModulePath}/cmake/CMakeLists.txt)
        ADD_SUBDIRECTORY(${ModulePath}/cmake)
    ELSE()
        MESSAGE(FATAL_ERROR "ModuleImport ${ModuleName} CMakeLists.txt not exist.")
    ENDIF()

    IF (WIN32)
        INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/${ModulePath}/src/windows)
        INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/${ModulePath}/src/${ModuleName}/windows)
    ENDIF(WIN32)
    INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/${ModulePath})
    INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/${ModulePath}/src)
    INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/${ModulePath}/src/${ModuleName})

    INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/${ModulePath}/include)
    INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/${ModulePath}/include/${ModuleName})

    INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/${ModulePath}/test)
    INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/${ModulePath}/test/${ModuleName})
endmacro(ModuleImport)
