file(GLOB_RECURSE sdk_files "sdk/*.py")

# base plugin

set(plugin_file "base.tcplug")
set(plugin_dest_dir "${trollcube_server_dir}/plugins/")
file(MAKE_DIRECTORY ${plugin_dest_dir})
set(plugin_dest_file "${plugin_dest_dir}${plugin_file}")
set(plugin_source_dir "${CMAKE_CURRENT_SOURCE_DIR}/plugins/base/")
file(GLOB_RECURSE plugin_source_files "plugins/base/*")

message(" == Plugin destination directory: ${plugin_dest_dir}")
message(" == Plugin destination file: ${plugin_dest_file}")
message(" == Plugin source directory: ${plugin_source_dir}")

set(plugin_creator "${CMAKE_CURRENT_SOURCE_DIR}/plugin_creator.py")

add_custom_command(
    OUTPUT "${plugin_dest_file}"
    COMMAND python2 "${plugin_creator}" --fullname "Trollcube official base plugin" --identifier "base" "${plugin_source_dir}" "${plugin_dest_dir}"
    DEPENDS "${plugin_creator}" ${plugin_source_files} ${sdk_files}
)
add_custom_target(base_tcplug ALL DEPENDS "${plugin_dest_file}") # "${plugin_creator}" ${plugin_source_files})


# test_world

set(world_file "test_world.tcworld")
set(world_dest_dir "${trollcube_server_dir}/worlds/test_world/")
file(MAKE_DIRECTORY ${world_dest_dir})
set(world_dest_file "${world_dest_dir}${world_file}")
set(world_source_dir "${plugin_dest_dir}")
file(GLOB_RECURSE world_source_files "${world_source_dir}/*")

message(" == World destination directory: ${world_dest_dir}")
message(" == World destination file: ${world_dest_file}")
message(" == World source directory: ${world_source_dir}")

set(plugin_installer "${CMAKE_CURRENT_SOURCE_DIR}/plugin_installer.py")

add_custom_command(
    OUTPUT "${world_dest_file}"
    COMMAND python2 "${plugin_installer}" --fullname "Dev Test World" --identifier "test_world" --recreate "${world_dest_file}" "${world_source_dir}"
    DEPENDS ${plugin_installer} ${world_source_files} ${sdk_files}
)
add_custom_target(test_world ALL DEPENDS "${world_dest_file}")# "${plugin_installer}" ${world_source_files})
add_dependencies(test_world base_tcplug)
