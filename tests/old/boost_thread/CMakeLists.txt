
cmake_minimum_required(VERSION 2.8)

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
)

# Sources
file(GLOB boost_thread_base_src "*.[ch]pp")

list(APPEND boost_thread_src
    ${boost_thread_base_src}
)

set(ALL_INCLUDES
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${Boost_INCLUDE_DIR}
)

add_executable(boost_thread
    ${boost_thread_src}
    ${common_src}
)
target_link_libraries(boost_thread ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
