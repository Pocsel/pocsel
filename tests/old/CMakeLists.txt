
# cmake -DCMAKE_CXX_COMPILER=clang++
#

enable_testing(true)

macro(simple_test test_name args regex srcs)
    list(APPEND ${test_name}_srcs "${srcs};${test_name}.cpp")
    add_executable(test_${test_name} ${${test_name}_srcs})
    add_test(${test_name} test_${test_name} ${args})
    set_tests_properties(${test_name} PROPERTIES PASS_REGULAR_EXPRESSION ${regex})
    list(APPEND TEST_EXECUTABLES test_${test_name})
endmacro(simple_test)

###############################################################################
# testing whether or testing system works
simple_test(testing_tests "" "Tests are working.\n" "")

###############################################################################
# testing ToString()
simple_test(ToString "" "tests done." "")

###############################################################################
# testing sqlite implem
include_directories(${SQLITE3_INCLUDE_DIR})
simple_test(sqlite "" "no args" "${db_src}")
target_link_libraries(test_sqlite libtools_db ${SQLITE3_LIBRARIES})
add_test(sqlite_insert test_sqlite "insert into users (login, password) values (\"pif\", \"paf\")")
add_test(sqlite_select test_sqlite "select password from users where login = \"pif\"" s)
add_test(sqlite_select_all test_sqlite "select * from users where login = \"pif\"" iss)

###############################################################################
# testing chunk coords to di translations
simple_test(Chunk "" "tests done." "Assert.hpp;../common/BaseChunk.hpp")

###############################################################################
# testing event manager
simple_test(EventManager "" "tests done." "Assert.hpp;../server/EventManager.hpp;../server/EventManager.cpp")
message("BOOST = ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT}")
target_link_libraries(test_EventManager ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})



###############################################################################
# testing message queue
include_directories(${Boost_INCLUDE_DIR})
set(message_queue_src
        ../tools/AbstractMessageQueue.hpp
        ../tools/AbstractMessageQueue.cpp
        ../tools/MessageQueue.hpp
        ../tools/MessageQueueManager.hpp
        ../tools/MessageQueueManager.cpp
        ../tools/Timer.hpp
        ../tools/Timer.cpp
)
simple_test(MessageQueue "" "tests done." "${message_queue_src}")
target_link_libraries(test_MessageQueue ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

###############################################################################
# testing renderers
#include_directories(${OPENGL_INCLUDE_DIR} ${GLEW_INCLUDE_DIR} ${IL_INCLUDE_DIR} ${FREETYPE_INCLUDE_DIRS})
#source_group("tools\\renderers" FILES ${tools_renderers_src})
#source_group("tools\\renderers\\opengl" FILES ${tools_renderers_opengl_src})
#list(APPEND renderers_src
#    ${tools_gui_src}
#    ${tools_gui_events_src}
#    ${tools_gui_primitives_src}
#    ${tools_renderers_src}
#    ${tools_renderers_opengl_src}
#    ../tools/Color.hpp
#    ../tools/Color.cpp
#    ../tools/Timer.hpp
#    ../tools/Timer.cpp
#    Renderers.cpp
#)
#simple_test(Renderers "" "tests done." "${renderers_src}")
#target_link_libraries(test_Renderers ${Boost_LIBRARIES} ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES} ${IL_LIBRARIES} ${ILU_LIBRARIES} ${ILUT_LIBRARIES} ${FREETYPE_LIBRARIES} ${SDL_LIBRARY})


###############################################################################
# testing lua
simple_test(lua "" "tests done" "")
target_link_libraries(test_lua libtools_lua ${LUA_LIBRARY})

simple_test(lua_noscript "" "tests done" "")
target_link_libraries(test_lua_noscript libtools_lua ${LUA_LIBRARY})

simple_test(lua_cubetype "" "tests done" "")
target_link_libraries(test_lua_cubetype libtools_lua ${LUA_LIBRARY})

simple_test(lua_bind "" "tests done" "")
target_link_libraries(test_lua_bind libtools_lua ${LUA_LIBRARY})

###############################################################################
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})
add_custom_target(test COMMAND ${CMAKE_CTEST_COMMAND} DEPENDS ${TEST_EXECUTABLES})
