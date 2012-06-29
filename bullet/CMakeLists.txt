file(GLOB_RECURSE bullet_src "*.[ch]pp" "*.h")

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DBT_USE_DOUBLE_PRECISION")
if (UNIX)
    string(REPLACE "-Wall" "" tmpReplace0 ${CMAKE_CXX_FLAGS})
    string(REPLACE "-Wextra" "" tmpReplace1 ${tmpReplace0})
    string(REPLACE "-Wshadow" "" tmpReplace2 ${tmpReplace1})
    set(CMAKE_CXX_FLAGS ${tmpReplace2})
else (UNIX)
#essayer de virer les warnings
endif (UNIX)

set(src ${bullet_src})

CreateGroups("${src}")
add_library(libbullet
    ${src}
)

target_link_libraries(libbullet)
