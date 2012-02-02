
# Packages
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
find_package(Boost COMPONENTS filesystem system date_time regex thread REQUIRED)
find_package(Threads)


set(ALL_INCLUDES
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${Boost_INCLUDE_DIR}
    #${SQLITE3_INCLUDE_DIR}
    #${LUA_INCLUDE_DIR}
    #${LUABIND_INCLUDE_DIR}
)

include_directories(
    ${ALL_INCLUDES}
)

# Sources
file(GLOB_RECURSE server_src "*.[ch]pp")
CreateGroups("${server_src}")

add_executable(server ${server_src})

enable_pch(server)

target_link_libraries(server libcommon libtools_base libtools_db libtools_lua
    ${Boost_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
)
