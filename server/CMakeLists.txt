file(GLOB_RECURSE server_src "*.[ch]pp")

set(src
    ${server_src}
    ${common_src}
    ${tools_base_src}
    ${tools_database_src}
    ${tools_logger_src}
    ${tools_lua_src}
    ${tools_lua_utils_src}
    ${tools_zlib_src}
    ${tools_plugin_src}
    ${tools_physics_src}
)

CreateGroups("${src}")
add_executable(server ${src})

enable_pch(server src)

target_link_libraries(server
    ${Boost_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
    ${LUA_LIBRARIES}
    ${SQLITE3_LIBRARIES}
    ${ZLIB_LIBRARIES}
)

SetProgramInfo(server)
add_dependencies(server dev_world)