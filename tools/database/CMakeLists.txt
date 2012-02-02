
file(GLOB tools_database_src
    "*.[ch]pp"
    "sqlite/*.[ch]pp"
)
source_group("tools\\database" FILES ${tools_database_src})

find_package(Sqlite3 REQUIRED)

include_directories(
    ${SQLITE3_INCLUDE_DIR}
)

add_library(libtools_db ${tools_database_src})

target_link_libraries(libtools_db ${SQLITE3_LIBRARIES})
