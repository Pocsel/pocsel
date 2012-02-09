macro(CreateGroups src)

    if (MSVC)
        foreach (pathSourceFile ${src})

            get_filename_component (pathSourceGroup ${pathSourceFile} PATH)

            if (NOT ${pathSourceGroup} STREQUAL ${CMAKE_SOURCE_DIR})

                # Make the path relative to the base source path
                string (REPLACE ${CMAKE_SOURCE_DIR} "" pathSourceGroup ${pathSourceGroup})

                # Remove starting / and \
                string (SUBSTRING ${pathSourceGroup} 0 1 firstChar)
                if ((firstChar STREQUAL "/") OR (firstChar STREQUAL "\\"))
                    string (LENGTH ${pathSourceGroup} strLen)
                    math (EXPR strLen "${strLen} - 1")
                    string (SUBSTRING ${pathSourceGroup} 1 ${strLen} pathSourceGroup)
                endif()

                # Replace / with \ so the source groups work correctly in MSVC
                string (REPLACE "/" "\\" pathSourceGroup ${pathSourceGroup})

                # Add file to the source group
                get_filename_component (fileExtension ${pathSourceFile} EXT)
                source_group ("${pathSourceGroup}" FILES ${pathSourceFile})

            endif()

        endforeach()
    endif()

endmacro()