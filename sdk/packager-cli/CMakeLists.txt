file(GLOB_RECURSE packager-cli_src "*.[ch]pp")

set(src
    ${packager-cli_src}
    ${common_src}
    ${tools_base_src}
    ${tools_database_src}
    ${tools_logger_src}
    ${tools_lua_src}
    ${tools_plugin_create_src}
)

CreateGroups("${src}")
add_executable(packager-cli ${src})
add_dependencies(packager-cli resources)

enable_pch(packager-cli src)

target_link_libraries(packager-cli
    ${Boost_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
    ${LUA_LIBRARIES}
    ${SQLITE3_LIBRARIES}
)

SetProgramInfo(packager-cli)
