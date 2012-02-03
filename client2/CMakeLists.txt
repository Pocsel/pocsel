file(GLOB_RECURSE client_src "*.[ch]pp")

set(src
	${client_src}
)

CreateGroups("${src}")
add_executable(client2 ${src})

enable_pch(client2 src)

target_link_libraries(client2
    ${Boost_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
	${SQLITE3_LIBRARIES}
	${LUA_LIBRARIES}
	${FREETYPE_LIBRARIES} ${IL_LIBRARIES} ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES} ${SDL_LIBRARY}
)
