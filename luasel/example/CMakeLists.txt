cmake_minimum_required(VERSION 2.8)

project(Example)

find_package(Lua51 REQUIRED)

if (UNIX)
    add_definitions(-std=c++0x)
endif (UNIX)

include_directories(${LUA_INCLUDE_DIR} "${CMAKE_CURRENT_SOURCE_DIR}/../include")

file(GLOB src "${CMAKE_CURRENT_SOURCE_DIR}/*.[ch]pp")

add_executable(example ${src})

target_link_libraries(example "${CMAKE_CURRENT_SOURCE_DIR}/../build/libluasel.a" ${LUA_LIBRARIES})
