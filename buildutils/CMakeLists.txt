file(GLOB_RECURSE buildutils_src "*.[ch]pp")

set(src
    ${buildutils_src}
)

CreateGroups("${src}")
add_executable(buildutils ${src})

#enable_pch(buildutils src)

#target_link_libraries(buildutils)

SetProgramInfo(buildutils)