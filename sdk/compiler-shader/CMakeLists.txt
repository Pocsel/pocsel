file(GLOB_RECURSE compiler-shader_src "*.[ch]pp")

set(src
    ${compiler-shader_src}
    ${tools_base_src}
    ${tools_logger_src}
)

CreateGroups("${src}")

add_executable(compiler-shader ${src})
if (MSVC)
    set_target_properties(compiler-shader PROPERTIES COMPILE_FLAGS "/bigobj")
endif (MSVC)

enable_pch(compiler-shader src)

target_link_libraries(compiler-shader
    ${Boost_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
    ${CG_LIBRARY}
)

SetProgramInfo(compiler-shader)
