file(GLOB_RECURSE respacker_src "*.[ch]pp")

set(src
    ${respacker_src}
)

CreateGroups("${src}")
add_executable(respacker ${src})

#enable_pch(respacker src)

#target_link_libraries(respacker)

#SetProgramInfo(respacker)