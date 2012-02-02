
file(GLOB_RECURSE tools_renderers_src "*.[ch]pp")
CreateGroups("${tools_renderers_src}")

include_directories(
    ${OPENGL_INCLUDE_DIR}
    ${GLEW_INCLUDE_DIR}
    ${IL_INCLUDE_DIR}
    ${FREETYPE_INCLUDE_DIRS}
)

add_library(libtools_renderers
    ${tools_renderers_src}
)

target_link_libraries(libtools_renderers
    ${FREETYPE_LIBRARIES}
    ${IL_LIBRARIES} ${ILU_LIBRARIES}
    ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES}
    ${SDL_LIBRARY}
)
