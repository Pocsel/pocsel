file(GLOB_RECURSE test_lua_src "*.[ch]pp")

set(src
    ${test_lua_src}
    ${common_src}
    ${tools_base_src}
    ${tools_lua_utils_src}
    ${tools_logger_src}
)

CreateGroups("${src}")
add_executable(test_lua ${src})
add_dependencies(test_lua tools_lua_res)

enable_pch(test_lua src)

target_link_libraries(test_lua
    ${Boost_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
    ${LUA_LIBRARIES}
    luasel
)

SetProgramInfo(test_lua)
