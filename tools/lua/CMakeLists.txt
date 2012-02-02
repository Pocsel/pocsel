
file(GLOB_RECURSE tools_lua_src "*.[ch]pp")
CreateGroups("${tools_lua_src}")

find_package(Lua51 REQUIRED)
include_directories(
    ${LUA_INCLUDE_DIR}
)

add_library(libtools_lua ${tools_lua_src})

target_link_libraries(libtools_lua
    ${LUA_LIBRARIES}
)
