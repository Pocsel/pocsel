file(GLOB_RECURSE test_src "*.[ch]pp")

set(src
    ${test_src}
    ${common_src}
    ${tools_base_src}
    ${tools_logger_src}
)

CreateGroups("${src}")
add_executable(test_logger ${src})

enable_pch(test_logger src)

target_link_libraries(test_logger
    ${Boost_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
    ${SQLITE3_LIBRARIES}
)

SetProgramInfo(test_logger)
