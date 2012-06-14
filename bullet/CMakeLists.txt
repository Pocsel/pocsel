file(GLOB_RECURSE bullet_src "*.[ch]pp" "*.h")

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

if (UNIX)
    string(REPLACE "-Wall" "" tmpReplace0 ${CMAKE_CXX_FLAGS})
    string(REPLACE "-Wextra" "" tmpReplace1 ${tmpReplace0})
    set(CMAKE_CXX_FLAGS ${tmpReplace1})
else (UNIX)
#essayer de virer les warnings
endif (UNIX)

set(src ${bullet_src})

CreateGroups("${src}")
add_library(libbullet
    ${src}
)

target_link_libraries(libbullet)
