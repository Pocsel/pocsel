
# Sources
file(GLOB_RECURSE server_src "*.[ch]pp")

set(src
    ${server_src}
    ${common_src}
    ${tools_base_src}
    ${tools_database_src}
    ${tools_lua_src}
)

CreateGroups("${src}")
add_executable(server ${src})

enable_pch(server src)

target_link_libraries(server
    ${Boost_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
    ${LUA_LIBRARIES}
    ${SQLITE3_LIBRARIES}
)
