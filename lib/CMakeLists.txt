cmake_minimum_required(VERSION 3.16)

project(
    lua
    VERSION 5.5.0
    LANGUAGES C
)

file(GLOB LUA_SOURCES "lua-5.5.0/src/*.c")
list(REMOVE_ITEM LUA_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/lua-5.5.0/src/lua.c" "${CMAKE_CURRENT_SOURCE_DIR}/lua-5.5.0/src/lua.c")
add_library(lua_lib STATIC ${LUA_SOURCES})
target_include_directories(lua_lib INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/src)
