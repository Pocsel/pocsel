find_package(Qt4 COMPONENTS QtCore QtGui REQUIRED)
file(GLOB qthpp "qt/*.hpp")
file(GLOB qtforms "ui/*.ui")
file(GLOB qtres "res/*.qrc")

include(${QT_USE_FILE})

file(GLOB_RECURSE packager-qt_src "*.[ch]pp")

set(src
    ${packager-qt_src}
    ${common_src}
    ${tools_base_src}
    ${tools_database_src}
    ${tools_logger_src}
    ${tools_lua_src}
    ${tools_plugin_create_src}
)

qt4_wrap_cpp(src ${qthpp})
qt4_wrap_ui(src ${qtforms})
qt4_add_resources(src ${qtres})

add_definitions(${QT_DEFINITIONS})

CreateGroups("${src}")
add_executable(packager-qt ${src} ${qtres})

enable_pch(packager-qt src)

target_link_libraries(packager-qt
    ${QT_LIBRARIES}
    ${Boost_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
    ${LUA_LIBRARIES}
    ${SQLITE3_LIBRARIES}
)

SetProgramInfo(packager-qt)
