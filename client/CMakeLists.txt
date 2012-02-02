
file(MAKE_DIRECTORY "${trollcube_client_dir}/cache/")

set(ALL_INCLUDES
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    #${OPENGL_INCLUDE_DIR}
    #${GLEW_INCLUDE_DIR}
    ${Boost_INCLUDE_DIR}
    #${IL_INCLUDE_DIR}
    #${FREETYPE_INCLUDE_DIRS}
    #${SQLITE3_INCLUDE_DIR}
)

include_directories(
    ${ALL_INCLUDES}
)

# Sources
file(GLOB_RECURSE client_src "*.[ch]pp")
CreateGroups("${client_src}")

add_executable(client ${client_src})

enable_pch(client)

#message("-- Link with ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES} ${Boost_LIBRARIES}")
target_link_libraries(client
    libcommon
    libtools_base
    libtools_renderers
    libtools_gui
    libtools_db
    libtools_lua
    ${Boost_LIBRARIES} # Utils
    ${CMAKE_THREAD_LIBS_INIT}
)

set(binds_file "${trollcube_client_dir}/binds.lua")

add_custom_command(
    OUTPUT "${binds_file}"
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/bindings.lua" "${trollcube_client_dir}"
    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/bindings.lua"
)

add_custom_target(binds ALL DEPENDS "${binds_file}")

