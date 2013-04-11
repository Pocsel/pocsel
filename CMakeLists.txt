cmake_minimum_required(VERSION 2.8)

project(Pocsel)

# Modules supplémentaires
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
include(CreateGroups)
include(CreateResourceProject)
include(CustomPrecompiledHeader)
include(GetGitRevision)
get_git_head_revision(GIT_REFSPEC GIT_SHA1)

# Packages
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
if (MSVC)
    set(Boost_COMPILER "-vc110")
endif (MSVC)
find_package(Boost COMPONENTS system filesystem thread date_time chrono regex program_options wave REQUIRED)
find_package(Threads)
find_package(DirectX REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(DevIL REQUIRED)
find_package(Freetype REQUIRED)
find_package(SDL REQUIRED)
find_package(Cg REQUIRED)
find_package(Lua51 REQUIRED)
find_package(Sqlite3 REQUIRED)
find_package(ZLIB REQUIRED)
find_package(FMODEx REQUIRED)

# configuration
if (UNIX)
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        add_definitions(-std=c++11)
        set(CMAKE_CXX_FLAGS "-Wall -Wextra -Winvalid-pch -msse2")
        set(CMAKE_CXX_FLAGS_DEBUG "-g -DDEBUG -D_DEBUG -UNDEBUG")
        set(CMAKE_CXX_FLAGS_RELEASE "-O3 -g -DNDEBUG -UDEBUG -U_DEBUG")
    else()
        add_definitions(-std=c++0x) #Enable C++0x mode
        set(CMAKE_CXX_CUSTOM_FLAGS "-Wall -Wextra -Winvalid-pch -msse2")
        set(CMAKE_CXX_CUSTOM_FLAGS_DEBUG "-g -DDEBUG -D_DEBUG -UNDEBUG")
        set(CMAKE_CXX_CUSTOM_FLAGS_RELEASE "-O3 -ffast-math -g -DNDEBUG -UDEBUG -U_DEBUG")
        #set(CMAKE_CXX_CUSTOM_FLAGS_RELEASE "-O3 -DNDEBUG -UDEBUG -m32") # cross compilation x86 sur un linux x64
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++")
    endif()
else (UNIX)
	if (MSVC)
		set(CMAKE_CXX_CUSTOM_FLAGS "/MT /MP -D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS")
		set(CMAKE_CXX_CUSTOM_FLAGS_DEBUG "-DDEBUG -D_DEBUG")
		set(CMAKE_CXX_CUSTOM_FLAGS_RELEASE "-DNDEBUG /arch:SSE2 /Ox /Ot /fp:fast")
		set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS} /DEBUG")
		set(CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO "${CMAKE_EXE_LINKER_FLAGS} /DEBUG")
	elseif (MINGW)
		add_definitions(-std=c++0x) #Enable C++0x mode
        set(CMAKE_CXX_CUSTOM_FLAGS "-Wall -Wextra -Winvalid-pch -msse2")
        set(CMAKE_CXX_CUSTOM_FLAGS_DEBUG "-g -DDEBUG -D_DEBUG -UNDEBUG")
        set(CMAKE_CXX_CUSTOM_FLAGS_RELEASE "-O3 -ffast-math -g -DNDEBUG -UDEBUG -U_DEBUG")
        #set(CMAKE_CXX_CUSTOM_FLAGS_RELEASE "-O3 -DNDEBUG -UDEBUG -m32") # cross compilation x86 sur un linux x64
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++")
	endif (MSVC)
endif (UNIX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_CUSTOM_FLAGS}")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${CMAKE_CXX_CUSTOM_FLAGS_DEBUG}")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${CMAKE_CXX_CUSTOM_FLAGS_RELEASE}")

set(CPACK_SOURCE_IGNORE_FILES "/\\\\..*$;~$;.swp$;${CPACK_SOURCE_IGNORE_FILES}")
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin")

set(ADD_FLAGS_ONCE true)
option(PLM_CONFIG_CLANG_COMPLETE "generate .clang_complete" OFF)

macro(SetProgramInfo target)
    include_directories(${CMAKE_CURRENT_BINARY_DIR})
    set(program_name "${target}")
    configure_file("${CMAKE_SOURCE_DIR}/ProgramInfo.hpp.in" "${CMAKE_CURRENT_BINARY_DIR}/ProgramInfo.hpp")
endmacro()

set(dev_conf_dir "${CMAKE_SOURCE_DIR}/confdir")
configure_file("common/DevConfDir.hpp.in" "${CMAKE_CURRENT_BINARY_DIR}/common/DevConfDir.hpp")

# fichiers
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
    ${DirectX_D3DX9_INCLUDE_DIR}

    ${SQLITE3_INCLUDE_DIR}
    ${LUA_INCLUDE_DIR}
    ${ZLIB_INCLUDE_DIR}
    ${FMODEX_INCLUDE_DIR}
    "${CMAKE_CURRENT_SOURCE_DIR}/luasel/include"
)
include_directories(
    ${ALL_INCLUDES}
)

message(${ALL_INCLUDES})

set(BULLET_DIR "${CMAKE_SOURCE_DIR}/bullet")

file(GLOB           common_src                  "common/*.[ch]pp")
file(GLOB           common_physics_src          "common/physics/*.[ch]pp")

file(GLOB           tools_base_src              "tools/*.[ch]pp" "tools/meta/*.[ch]pp")
file(GLOB_RECURSE   tools_database_src          "tools/database/*.[ch]pp")
file(GLOB_RECURSE   tools_gfx_src               "tools/gfx/*.[ch]pp")
file(GLOB_RECURSE   tools_gui_src               "tools/gui/*.[ch]pp")
file(GLOB_RECURSE   tools_logger_src            "tools/logger/*.[ch]pp")
file(GLOB_RECURSE   tools_lua_utils_src         "tools/lua/utils/*.[ch]pp")
file(GLOB_RECURSE   tools_plugin_install_src    "tools/plugin-install/*.[ch]pp")
file(GLOB_RECURSE   tools_plugin_create_src     "tools/plugin-create/*.[ch]pp")
file(GLOB_RECURSE   tools_renderers_src         "tools/renderers/*.[ch]pp")
file(GLOB           tools_models_src            "tools/models/*.[ch]pp")
file(GLOB           tools_models_convert_src    "tools/models/convert/*.[ch]pp")
file(GLOB_RECURSE   tools_stat_src              "tools/stat/*.[ch]pp")
file(GLOB_RECURSE   tools_thread_src            "tools/thread/*.[ch]pp")
file(GLOB_RECURSE   tools_window_src            "tools/window/*.[ch]pp")
file(GLOB_RECURSE   tools_zlib_src              "tools/zlib/*.[ch]pp")
file(GLOB_RECURSE   tools_sound_src             "tools/sound/*.[ch]pp")
# lua resources
file(GLOB_RECURSE   tools_lua_res               "tools/lua/utils/*.lua")

# resources
CreateResourceProject(tools_lua_res tools_lua_src "${tools_lua_res}")

add_subdirectory(luasel)
add_subdirectory(buildutils)
add_subdirectory(sdk)
add_subdirectory(plugins)
add_subdirectory(bullet)
add_subdirectory(client)
add_subdirectory(server)
add_subdirectory(tests)
