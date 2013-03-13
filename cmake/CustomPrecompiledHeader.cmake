# DON'T FORGET to include ${CMAKE_CURRENT_SOURCE_DIR} in include_directories for the compiler
# to see the header, and ${CMAKE_CURRENT_BINARY_DIR} for the compiler to see the GCC PCH

# "sources" - unexpanded cmake variable holding all the source files
# "includes" - unexpanded cmake variable holding all include paths the PCH needs to know about
# "target_name" - the name of the a special target used to build the PCH for GCC
# "header_name" - the name of the PCH header, without the extension; "stdafx" or something similar;
#                 note that the source file compiling the header needs to have the same name
# "project_name" - the name of the project that wants a PCH
macro(precompiled_header sources includes target_name header_name project_name)

    # MSVC precompiled headers cmake code
    if (MSVC)
    
#        if (CMAKE_BUILD_TOOL STREQUAL "nmake")
#
#        else ()
            set_source_files_properties(${header_name}.cpp PROPERTIES COMPILE_FLAGS "/Yc${header_name}.hpp")

            set(CMAKE_CXX_FLAGS "/FI\"${header_name}.hpp\" ${CMAKE_CXX_FLAGS} /Yu${header_name}.hpp")

            # ${header_name}.cpp has to come before ${header_name}.hpp,
            # otherwise we get a linker error...
            list(INSERT ${sources} 0 ${header_name}.hpp)
            list(INSERT ${sources} 0 ${header_name}.cpp)
#        endif ()

    # GCC precompiled headers cmake code
    # We don't do this on Macs since GCC there goes haywire
    # when you try to generate a PCH with two "-arch" flags
    elseif(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        #message("GCC PCH hook")
        # Get the compiler flags for this build type
        string(TOUPPER "CMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE}" flags_for_build_name)
        set(compile_flags ${${flags_for_build_name}})

        # Add all the Qt include directories
        foreach(item ${${includes}})
            list(APPEND compile_flags "-I${item}")
        endforeach()

        # Get the list of all build-independent preprocessor definitions
        get_directory_property(defines_global COMPILE_DEFINITIONS)
        list(APPEND defines ${defines_global})

        # Get the list of all build-dependent preprocessor definitions
        string(TOUPPER "COMPILE_DEFINITIONS_${CMAKE_BUILD_TYPE}" defines_for_build_name)
        get_directory_property(defines_build ${defines_for_build_name})
        list(APPEND defines ${defines_build})

        # Add the "-D" prefix to all of them
        foreach(item ${defines})
            list(APPEND all_define_flags "-D${item}")
        endforeach()

        if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
            list(APPEND compile_flags ${CMAKE_CXX_FLAGS} -std=c++11 ${all_define_flags})
        else()
            list(APPEND compile_flags ${CMAKE_CXX_FLAGS} -std=c++0x ${all_define_flags})
        endif()

        # Prepare the compile flags var for passing to GCC
        separate_arguments(compile_flags)

        # Finally, build the precompiled header.
        # We don't add the buil command to add_custom_target
        # because that would force a PCH rebuild even when
        # the ${header_name}.hpp file hasn't changed. We add it to
        # a special add_custom_command to work around this problem.
        add_custom_target(
            ${target_name} ALL
            DEPENDS ${header_name}.hpp.gch
        )

        add_custom_command(OUTPUT
            ${header_name}.hpp.gch
            COMMAND ${CMAKE_CXX_COMPILER} ${compile_flags} ${CMAKE_CURRENT_SOURCE_DIR}/${header_name}.hpp -o ${header_name}.hpp.gch
            MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/${header_name}.hpp
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            VERBATIM
        )

    # Xcode has PCH support
    elseif(APPLE)
        set_target_properties(
            ${project_name}
            PROPERTIES
            XCODE_ATTRIBUTE_GCC_PREFIX_HEADER "${header_name}.hpp"
            XCODE_ATTRIBUTE_GCC_PRECOMPILE_PREFIX_HEADER "YES"
       )
    endif()
endmacro()

macro(enable_pch target target_src)
    list(REMOVE_ITEM ${target_src} precompiled.cpp)
    set(GCC_PCH_TARGET ${target}_gccPCH_fc)
    precompiled_header(${target_src} ALL_INCLUDES ${GCC_PCH_TARGET} precompiled ${target})

    if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        if (ADD_FLAGS_ONCE)
            set(CMAKE_CXX_FLAGS "-I ${CMAKE_CURRENT_SOURCE_DIR} -include precompiled.hpp ${CMAKE_CXX_FLAGS}")
            configure_file(${CMAKE_CURRENT_SOURCE_DIR}/precompiled.hpp ${CMAKE_CURRENT_BINARY_DIR}/precompiled.hpp)
            include_directories(${CMAKE_CURRENT_SOURCE_DIR})
        endif()
    endif()
    if(PLM_CONFIG_CLANG_COMPLETE)
        if(ADD_FLAGS_ONCE)
            get_property(inc_dirs DIRECTORY PROPERTY INCLUDE_DIRECTORIES)
            set (file_clang_complete "${CMAKE_CURRENT_SOURCE_DIR}/.clang_complete")
            set (inc_args " -std=c++11\n")
            foreach (arg ${inc_dirs})
                set (inc_args "${inc_args} -I${arg}\n")
            endforeach ()
            set (inc_args "${inc_args} -include precompiled.cpp\n")
            file (WRITE "${file_clang_complete}" "${inc_args}")
            message (STATUS "Generated ${file_clang_complete}")
        endif()
    endif(PLM_CONFIG_CLANG_COMPLETE)
    set(ADD_FLAGS_ONCE false)
endmacro()
