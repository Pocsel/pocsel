
cmake_minimum_required(VERSION 2.8)

set(CMAKE_BUILD_TYPE Release)
# Packages
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
find_package(Boost COMPONENTS system thread date_time regex REQUIRED)
find_package(Threads)
find_package(Sqlite3 REQUIRED)


include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${Boost_INCLUDE_DIR}
    ${SQLITE3_INCLUDE_DIR}
    ..
    ../..
    ../../server
)

if(CMAKE_COMPILER_IS_GNUCXX)
    add_definitions(-std=c++0x) #Enable C++0x mode
endif()

if (UNIX)
    set(CMAKE_CXX_CUSTOM_FLAGS "-Wall -Wextra --include precompiled.hpp")
    set(CMAKE_CXX_CUSTOM_FLAGS_DEBUG "-g3 -DDEBUG")
    set(CMAKE_CXX_CUSTOM_FLAGS_RELEASE "-O3 -DNDEBUG") # doublon ? cmake le fait deja, mais on sait jamais..
else(UNIX)
    set(CMAKE_CXX_CUSTOM_FLAGS "/FI\"precompiled.hpp\"")
    set(CMAKE_CXX_CUSTOM_FLAGS_DEBUG "-DDEBUG")
    set(CMAKE_CXX_CUSTOM_FLAGS_RELEASE "-DNDEBUG")
endif(UNIX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_CUSTOM_FLAGS}")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${CMAKE_CXX_CUSTOM_FLAGS_DEBUG}")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${CMAKE_CXX_CUSTOM_FLAGS_RELEASE}")

file(GLOB perlin_base_src "*.[ch]pp" "*.[ch]")
file(GLOB perlin_srv_src
    "../../server/mapgen/Perlin.[ch]pp"
    "../../server/mapgen/IRand.[ch]pp"
    "../../server/mapgen/RandMersenneTwister.[ch]pp")
file(GLOB common_src "../../common/*.[ch]pp")

list(APPEND perlin_src
    ${perlin_base_src}
    ${perlin_srv_src}
)

set(ALL_INCLUDES
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${Boost_INCLUDE_DIR}
)

add_executable(perlin
    ${perlin_src}
    ${common_src}
)
target_link_libraries(perlin ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
