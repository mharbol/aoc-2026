
project(
    lua_lib_intf
    VERSION 0.0.1
    LANGUAGES CXX
)

add_library(
    ${PROJECT_NAME}
    src/LuaIntf.cpp
)

target_include_directories(
    ${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
