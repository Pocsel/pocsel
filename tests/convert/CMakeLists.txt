file(GLOB_RECURSE test_src "*.[ch]pp")


set(src
    ${test_src}
    ${common_src}
    ${tools_base_src}
    ${tools_models_src}
    ${tools_logger_src}
)

CreateGroups("${src}")
add_executable(test_convert ${src})

enable_pch(test_convert src)

target_link_libraries(test_convert
    ${Boost_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
)

SetProgramInfo(test_convert)
