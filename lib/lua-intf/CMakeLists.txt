
project(
    lua_lib_intf
    VERSION 0.0.1
    LANGUAGES CXX
)

add_subdirectory(src)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME} lua_lib_core)
