
file(GLOB_RECURSE common_src "*.[ch]pp")
CreateGroups("${common_src}")

add_library(libcommon ${common_src})

#target_link_libraries(libcommon)
