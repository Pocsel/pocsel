file(MAKE_DIRECTORY "${pocsel_client_dir}/cache/")

# Sources
file(GLOB_RECURSE client_src "*.[ch]pp")

set(src
    ${client_src}
    ${common_src}
    ${tools_base_src}
    ${tools_database_src}
    ${tools_gui_src}
    ${tools_lua_src}
    ${tools_renderers_src}
)

CreateGroups("${src}")
add_executable(client ${src})

enable_pch(client)

#message("-- Link with ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES} ${Boost_LIBRARIES}")
target_link_libraries(client
    ${Boost_LIBRARIES} # Utils
    ${CMAKE_THREAD_LIBS_INIT}
    ${SQLITE3_LIBRARIES}
    ${LUA_LIBRARIES}
    ${FREETYPE_LIBRARIES} ${IL_LIBRARIES} ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES} ${SDL_LIBRARY} # gui
)

set(binds_file "${pocsel_client_dir}/bindings.lua")

add_custom_command(
    OUTPUT "${binds_file}"
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/bindings.lua" "${pocsel_client_dir}"
    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/bindings.lua"
)

add_custom_target(binds ALL DEPENDS "${binds_file}")
