file(GLOB_RECURSE server_src "*.[ch]pp")

include_directories(${BULLET_DIR})

set(src
    ${server_src}
    ${common_src}
    ${common_physics_src}
    ${tools_base_src}
    ${tools_database_src}
    ${tools_logger_src}
    ${tools_lua_utils_src}
    ${tools_zlib_src}
    ${tools_plugin_src}
    ${tools_stat_src}
)

CreateGroups("${src}")
add_executable(server ${src})
add_dependencies(server tools_lua_res)

enable_pch(server src)

target_link_libraries(server
    libbullet
    luasel
    ${Boost_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
    ${LUA_LIBRARIES}
    ${SQLITE3_LIBRARIES}
    ${ZLIB_LIBRARIES}
)

SetProgramInfo(server)
add_dependencies(server dev_world)
