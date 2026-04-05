cmake_minimum_required(VERSION 3.16)

project(
    lua
    VERSION 5.5.0
    LANGUAGES C
)

set(LUA_DIR "lua-${PROJECT_VERSION}")

file(GLOB LUA_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/${LUA_DIR}/src/*.c")
list(
    REMOVE_ITEM LUA_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/${LUA_DIR}/src/lua.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/${LUA_DIR}/src/lua.c"
)

add_library(lua_lib STATIC ${LUA_SOURCES})
target_include_directories(lua_lib INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/${LUA_DIR}/src)
