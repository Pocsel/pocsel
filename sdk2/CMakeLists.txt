find_package(Qt4 COMPONENTS QtCore QtGui QtNetwork REQUIRED)
set(QT_USE_QTNETWORK TRUE)
file(GLOB qthpp "qt/*.hpp")
file(GLOB qtforms "ui/*.ui")
file(GLOB qtres "res/*.qrc")

include(${QT_USE_FILE})

file(GLOB_RECURSE sdk_src "*.[ch]pp")

set(src
    ${sdk_src}
    ${common_src}
    ${tools_base_src}
    ${tools_database_src}
    ${tools_logger_src}
    ${tools_lua_src}
    ${tools_plugin_src}
)

qt4_wrap_cpp(src ${qthpp})
qt4_wrap_ui(src ${qtforms})
qt4_add_resources(src ${qtres})

add_definitions(${QT_DEFINITIONS})

CreateGroups("${src}")
add_executable(sdk ${src} ${qtres})

enable_pch(sdk src)

target_link_libraries(sdk
    ${QT_LIBRARIES}
    ${Boost_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
    ${LUA_LIBRARIES}
    ${SQLITE3_LIBRARIES}
)

SetProgramInfo(sdk)
