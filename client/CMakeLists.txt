file(GLOB_RECURSE client_src "*.[ch]pp")
file(GLOB_RECURSE resources_lua_src "*.lua")
file(GLOB_RECURSE resources_fx_src "*.fx")

include_directories(${BULLET_DIR})

set(src
    ${client_src}
    ${common_src}
    ${common_physics_src}
    ${tools_base_src}
    ${tools_database_src}
    ${tools_logger_src}
    ${tools_lua_src}
    ${tools_lua_utils_src}
    ${tools_renderers_src}
    ${tools_models_src}
    ${tools_stat_src}
    ${tools_thread_src}
    ${tools_window_src}
)
set(res_src
    ${resources_lua_src}
    ${resources_fx_src}
)

# Resources du client
CreateResourceProject(client_resources src "${res_src}")

add_definitions(-DTOOLS_WINDOW_BINDACTION_HEADER="client/BindAction.hpp")

CreateGroups("${src}")
add_executable(client ${src})
add_dependencies(client tools_lua_res client_resources)

enable_pch(client src)

target_link_libraries(client
    libbullet
    ${Boost_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
    ${SQLITE3_LIBRARIES}
    ${LUA_LIBRARIES}
    ${FREETYPE_LIBRARIES}
    ${IL_LIBRARIES}
    ${ILU_LIBRARIES}
    ${OPENGL_LIBRARIES}
    ${GLEW_LIBRARIES}
    ${SDL_LIBRARY}
    ${CG_LIBRARY}
    ${CGGL_LIBRARY}
    ${DirectX_ERROR_LIBRARY}
    ${DirectX_D3D9_LIBRARY}
    ${DirectX_D3DX9_LIBRARY}
)

SetProgramInfo(client)
