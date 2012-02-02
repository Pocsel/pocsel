
file(GLOB_RECURSE tools_gui_src "*.[ch]pp")
CreateGroups("${tools_gui_src}")

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(DevIL REQUIRED)
find_package(Freetype REQUIRED)
find_package(SDL REQUIRED)

include_directories(
    ${OPENGL_INCLUDE_DIR}
    ${GLEW_INCLUDE_DIR}
    ${IL_INCLUDE_DIR}
    ${FREETYPE_INCLUDE_DIRS}
)

add_library(libtools_gui
    ${tools_gui_src}
)

target_link_libraries(libtools_gui libtools_base libtools_renderers
    ${FREETYPE_LIBRARIES}
    ${IL_LIBRARIES} ${ILU_LIBRARIES}
    ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES}
    ${SDL_LIBRARY}
)
