
project(
    lua_lib_intf
    VERSION 0.0.1
    LANGUAGES CXX
)

add_subdirectory(src)

target_include_directories(lua_lib_intf PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(lua_lib_intf lua_lib_core)
