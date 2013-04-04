file(GLOB_RECURSE test_shaders_src "*.[ch]pp")

set(src
    ${test_shaders_src}
    ${common_src}
    ${tools_base_src}
    ${tools_logger_src}
    ${tools_gfx_src}
    ${tools_window_src}
)

add_definitions(-DTOOLS_WINDOW_BINDACTION_HEADER="tests/shaders/BindAction.hpp")

CreateGroups("${src}")
add_executable(test_shaders ${src})

enable_pch(test_shaders src)

target_link_libraries(test_shaders
    luasel
    ${Boost_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
    ${LUA_LIBRARIES}
    ${FREETYPE_LIBRARIES}
    ${IL_LIBRARIES}
    ${ILU_LIBRARIES}
    ${OPENGL_LIBRARIES}
    ${GLEW_LIBRARIES}
    ${SDL_LIBRARY}
    ${DirectX_ERROR_LIBRARY}
    ${DirectX_D3D9_LIBRARY}
    ${DirectX_D3DX9_LIBRARY}
)

SetProgramInfo(test_shaders)
