file(GLOB_RECURSE client_src "*.[ch]pp")

set(src
    ${client_src}
    ${common_src}
    ${tools_base_src}
    ${tools_database_src}
    ${tools_logger_src}
    ${tools_lua_src}
    ${tools_renderers_src}
    ${tools_thread_src}
)

CreateGroups("${src}")
add_executable(client ${src})

enable_pch(client src)

target_link_libraries(client
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
	${CGDX9_LIBRARY}
	${DirectX_D3D9_LIBRARY}
)

SetProgramInfo(client)
