
# cmake -DCMAKE_CXX_COMPILER=clang++
#

cmake_minimum_required(VERSION 2.8)

project(Trollcube)

# Changer Debug en Release pour compiler en Release (SANS BLAGUE !!)
set(CMAKE_BUILD_TYPE Debug)

# Modules supplémentaires
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
include(${CMAKE_SOURCE_DIR}/cmake/CustomPrecompiledHeader.cmake )
include(${CMAKE_SOURCE_DIR}/cmake/CreateGroups.cmake)
#include(${CMAKE_SOURCE_DIR}/cmake/FindLuabind.cmake)
include(FindLua51)

# Packages
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost COMPONENTS system filesystem thread date_time regex program_options REQUIRED)
find_package(Threads)

if (CMAKE_COMPILER_IS_GNUCXX)
    add_definitions(-std=c++0x) #Enable C++0x mode
endif ()

if (UNIX)
    set(CMAKE_CXX_CUSTOM_FLAGS "-Wall -Wextra -Winvalid-pch --include tools/types.hpp --include tools/Delete.hpp")
    set(CMAKE_CXX_CUSTOM_FLAGS_DEBUG "-g -DDEBUG -UNDEBUG")
    set(CMAKE_CXX_CUSTOM_FLAGS_RELEASE "-O3 -DNDEBUG -UDEBUG -march=native -mtune=native")
else (UNIX)
    set(CMAKE_CXX_CUSTOM_FLAGS "/FI\"tools/types.hpp\" /FI\"tools/Delete.hpp\"")
    set(CMAKE_CXX_CUSTOM_FLAGS_DEBUG "-DDEBUG")
    set(CMAKE_CXX_CUSTOM_FLAGS_RELEASE "-DNDEBUG /arch:SSE2")
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS} /DEBUG")
    set(CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO "${CMAKE_EXE_LINKER_FLAGS} /DEBUG")
endif (UNIX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_CUSTOM_FLAGS}")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${CMAKE_CXX_CUSTOM_FLAGS_DEBUG}")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${CMAKE_CXX_CUSTOM_FLAGS_RELEASE}")

include_directories(. ${CMAKE_CURRENT_BINARY_DIR})

# Mets les includes où il faut quand il y a besoin, j'ai la flemme de faire la liste maintenant (surtout que sous linux, vous avez tout de manière crade au même endroit -_-)
# ha ok je viens de comprendre  ...
set(ALL_INCLUDES
#    ${CMAKE_SOURCE_DIR}
#    ${CMAKE_CURRENT_SOURCE_DIR}
#    ${CMAKE_CURRENT_BINARY_DIR}
    ${OPENGL_INCLUDE_DIR} #gui + precompiled.hpp
    ${GLEW_INCLUDE_DIR} #gui + precompiled.hpp
    ${Boost_INCLUDE_DIR} # le plus chiant => boost::noncopyable souvent utilisé
#    ${IL_INCLUDE_DIR}
#    ${FREETYPE_INCLUDE_DIRS}
    ${SQLITE3_INCLUDE_DIR} # sqlite seulement je crois
#    ${LUA_INCLUDE_DIR}
#    ${LUABIND_INCLUDE_DIR}
    ${SDL_INCLUDE_DIR} #gui seulement je crois
)
include_directories(
    ${ALL_INCLUDES}
)

set(trollcube_binaries_dir "${CMAKE_BINARY_DIR}/trollcube/binaries")
set(trollcube_server_dir "${CMAKE_BINARY_DIR}/trollcube/server")
set(trollcube_client_dir "${CMAKE_BINARY_DIR}/trollcube/client")
set(EXECUTABLE_OUTPUT_PATH ${trollcube_binaries_dir})

macro(enable_pch target)
    list(REMOVE_ITEM ${target}_src precompiled.cpp)
    set(GCC_PCH_TARGET ${target}_gccPCH_fc)
    precompiled_header(${target}_src ALL_INCLUDES ${GCC_PCH_TARGET} precompiled ${target})

    if(CMAKE_COMPILER_IS_GNUCXX)
        set(CMAKE_CXX_FLAGS "-I ${CMAKE_CURRENT_SOURCE_DIR} --include precompiled.hpp ${CMAKE_CXX_FLAGS}")
        configure_file(${CMAKE_CURRENT_SOURCE_DIR}/precompiled.hpp ${CMAKE_CURRENT_BINARY_DIR}/precompiled.hpp)
        include_directories(${CMAKE_CURRENT_SOURCE_DIR})
    endif()
endmacro()


add_subdirectory(tools)
add_subdirectory(common)
add_subdirectory(resources)
add_subdirectory(sdk)
add_subdirectory(client)
add_subdirectory(server)
add_subdirectory(tests)
