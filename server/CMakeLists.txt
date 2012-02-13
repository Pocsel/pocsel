
# Sources
file(GLOB_RECURSE server2_src "*.[ch]pp")

set(src
    ${server2_src}
    ${common_src}
    ${tools_base_src}
    ${tools_database_src}
    ${tools_logger_src}
    ${tools_lua_src}
)

CreateGroups("${src}")
add_executable(server2 ${src})

enable_pch(server2 src)

target_link_libraries(server2
    ${Boost_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
    ${LUA_LIBRARIES}
    ${SQLITE3_LIBRARIES}
)

SetProgramInfo(server2)