file(GLOB_RECURSE installer-cli_src "*.[ch]pp")

set(src
    ${installer-cli_src}
    ${common_src}
    ${tools_base_src}
    ${tools_database_src}
    ${tools_logger_src}
    ${tools_plugin_install_src}
)

CreateGroups("${src}")
add_executable(installer-cli ${src})
add_dependencies(installer-cli tools_lua_res)

enable_pch(installer-cli src)

target_link_libraries(installer-cli
    luasel
    ${Boost_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
    ${LUA_LIBRARIES}
    ${SQLITE3_LIBRARIES}
)

SetProgramInfo(installer-cli)
