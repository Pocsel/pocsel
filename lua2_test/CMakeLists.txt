file(GLOB_RECURSE lua2_test_src "*.[ch]pp")

set(src
    ${lua2_test_src}
    ${common_src}
    ${tools_base_src}
    ${tools_lua_src}
    ${tools_logger_src}
)

CreateGroups("${src}")
add_executable(lua2_test ${src})

enable_pch(lua2_test src)

target_link_libraries(lua2_test
    ${Boost_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
    ${LUA_LIBRARIES}
)

SetProgramInfo(lua2_test)
