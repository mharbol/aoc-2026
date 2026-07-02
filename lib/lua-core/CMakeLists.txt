
project(
    lua
    VERSION 5.5.0
    LANGUAGES C
)

set(LUA_DIR "lua-${PROJECT_VERSION}")
set(LUA_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/${LUA_DIR}/src")

file(GLOB LUA_SOURCES "${LUA_SRC_DIR}/*.c")
list(
    REMOVE_ITEM LUA_SOURCES
    "${LUA_SRC_DIR}/lua.c"
    "${LUA_SRC_DIR}/luac.c"
)

add_library(lua_lib_core STATIC ${LUA_SOURCES})
target_include_directories(lua_lib_core INTERFACE ${LUA_SRC_DIR})
