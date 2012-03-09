cmake_minimum_required(VERSION 2.8)

project(Pocsel)

# Modules supplémentaires
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
include(CustomPrecompiledHeader)
include(CreateGroups)
include(GetGitRevision)
get_git_head_revision(GIT_REFSPEC GIT_SHA1)

# Packages
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
find_package(Boost COMPONENTS system filesystem thread date_time regex program_options REQUIRED)
find_package(Threads)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(DevIL REQUIRED)
find_package(Freetype REQUIRED)
find_package(SDL REQUIRED)
find_package(Cg REQUIRED)
find_package(Lua51 REQUIRED)
find_package(Sqlite3 REQUIRED)
find_package(ZLIB REQUIRED)

if (UNIX)
    add_definitions(-std=c++0x) #Enable C++0x mode
    set(CMAKE_CXX_CUSTOM_FLAGS "-Wall -Wextra -Winvalid-pch")
    set(CMAKE_CXX_CUSTOM_FLAGS_DEBUG "-g -DDEBUG -UNDEBUG")
    set(CMAKE_CXX_CUSTOM_FLAGS_RELEASE "-O3 -ffast-math -g -DNDEBUG -UDEBUG -march=native -mtune=native")
    #set(CMAKE_CXX_CUSTOM_FLAGS_RELEASE "-O3 -DNDEBUG -UDEBUG -m32") # cross compilation x86 sur un linux x64
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++")
else (UNIX)
    set(CMAKE_CXX_CUSTOM_FLAGS "/MP")
    set(CMAKE_CXX_CUSTOM_FLAGS_DEBUG "-DDEBUG")
    set(CMAKE_CXX_CUSTOM_FLAGS_RELEASE "-DNDEBUG /arch:SSE2")
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS} /DEBUG")
    set(CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO "${CMAKE_EXE_LINKER_FLAGS} /DEBUG")
endif (UNIX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_CUSTOM_FLAGS}")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${CMAKE_CXX_CUSTOM_FLAGS_DEBUG}")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${CMAKE_CXX_CUSTOM_FLAGS_RELEASE}")

include_directories(. ${CMAKE_CURRENT_BINARY_DIR})

set(ALL_INCLUDES
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${Boost_INCLUDE_DIR}
    # gui
    ${OPENGL_INCLUDE_DIR}
    ${GLEW_INCLUDE_DIR}
    ${IL_INCLUDE_DIR}
    ${FREETYPE_INCLUDE_DIRS}
    ${SDL_INCLUDE_DIR}
    ${CG_INCLUDE_DIR}

    ${SQLITE3_INCLUDE_DIR}
    ${LUA_INCLUDE_DIR}
    ${ZLIB_INCLUDE_DIR}
)
include_directories(
    ${ALL_INCLUDES}
)

file(GLOB_RECURSE common_src "common/*.[ch]pp")
file(GLOB tools_base_src "tools/*.[ch]pp" "tools/meta/*.[ch]pp")
file(GLOB tools_database_src "tools/database/*.[ch]pp" "tools/database/sqlite/*.[ch]pp")
file(GLOB_RECURSE tools_gui_src "tools/gui/*.[ch]pp")
file(GLOB_RECURSE tools_lua_src "tools/lua/*.[ch]pp")
file(GLOB_RECURSE tools_renderers_src "tools/renderers/*.[ch]pp")
file(GLOB_RECURSE tools_thread_src "tools/thread/*.[ch]pp")
file(GLOB_RECURSE tools_logger_src "tools/logger/*.[ch]pp")
file(GLOB_RECURSE tools_zlib_src "tools/zlib/*.[ch]pp")

set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin")

macro(enable_pch target target_src)
    list(REMOVE_ITEM ${target_src} precompiled.cpp)
    set(GCC_PCH_TARGET ${target}_gccPCH_fc)
    precompiled_header(${target_src} ALL_INCLUDES ${GCC_PCH_TARGET} precompiled ${target})

    if(CMAKE_COMPILER_IS_GNUCXX)
        set(CMAKE_CXX_FLAGS "-I ${CMAKE_CURRENT_SOURCE_DIR} --include precompiled.hpp ${CMAKE_CXX_FLAGS}")
        configure_file(${CMAKE_CURRENT_SOURCE_DIR}/precompiled.hpp ${CMAKE_CURRENT_BINARY_DIR}/precompiled.hpp)
        include_directories(${CMAKE_CURRENT_SOURCE_DIR})
    endif()
endmacro()

macro(SetProgramInfo target)
    include_directories(${CMAKE_CURRENT_BINARY_DIR})
    set(program_name "${target}")
    configure_file("${CMAKE_SOURCE_DIR}/ProgramInfo.hpp.in" "${CMAKE_CURRENT_BINARY_DIR}/ProgramInfo.hpp")
endmacro()

set(dev_conf_dir "${CMAKE_SOURCE_DIR}/confdir")
configure_file("common/DevConfDir.hpp.in" "${CMAKE_CURRENT_BINARY_DIR}/common/DevConfDir.hpp")

add_subdirectory(sdk)
add_subdirectory(client)
add_subdirectory(server)
add_subdirectory(tests)
