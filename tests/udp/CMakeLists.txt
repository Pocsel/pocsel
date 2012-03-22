file(GLOB test_udp_sv_src "sv_*.[ch]pp" "tst_*.[ch]pp" "precompiled.[ch]pp")
file(GLOB test_udp_cl_src "cl_*.[ch]pp" "tst_*.[ch]pp" "precompiled.[ch]pp")

set(src_sv
    ${test_udp_sv_src}
    ${common_src}
    ${tools_base_src}
    ${tools_logger_src}
)

set(src_cl
    ${test_udp_cl_src}
    ${common_src}
    ${tools_base_src}
    ${tools_logger_src}
)

CreateGroups("${src}")
add_executable(test_udp_cl ${src_cl})
add_executable(test_udp_sv ${src_sv})

enable_pch(test_udp_sv src_sv)
enable_pch(test_udp_cl src_cl)

target_link_libraries(test_udp_cl
    ${Boost_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
)

target_link_libraries(test_udp_sv
    ${Boost_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
)

SetProgramInfo(test_udp_cl)
SetProgramInfo(test_udp_sv)
