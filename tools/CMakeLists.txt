
file(GLOB tools_base_src "*.[ch]pp")
file(GLOB tools_meta_src "meta/*.[ch]pp")
CreateGroups("${tools_base_src};${tools_meta_src}")

add_library(libtools_base ${tools_base_src} ${tools_meta_src})

add_subdirectory(renderers)
add_subdirectory(gui)
add_subdirectory(database)
add_subdirectory(lua)
