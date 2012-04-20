file(GLOB_RECURSE sdk_files "sdk/*.py")

# Plugins
# -------

set(plugin_creator "${CMAKE_CURRENT_SOURCE_DIR}/plugin_creator.py")
set(plugins_dest_dir "${CMAKE_CURRENT_SOURCE_DIR}/build")

macro(MakePlugin target filename fullname identifier source_dir)
    file(GLOB_RECURSE plugin_source_files "${source_dir}/*")
    set(plugin_dest_file "${plugins_dest_dir}/${filename}")
    add_custom_command(
        OUTPUT ${plugin_dest_file}
        COMMAND python2 "${plugin_creator}" --fullname "${fullname}" --identifier "${identifier}" "${source_dir}" "${plugins_dest_dir}"
        DEPENDS ${plugin_creator} ${plugin_source_files} ${sdk_files}
    )
    add_custom_target(${target} ALL DEPENDS ${plugin_creator} ${plugin_source_files} ${sdk_files} ${plugin_dest_file})
endmacro()

MakePlugin(base_plugin "base.tcplug" "Pocsel base plugin" "base" "${CMAKE_CURRENT_SOURCE_DIR}/plugins/base")

# Dev World
# ---------

set(plugin_installer "${CMAKE_CURRENT_SOURCE_DIR}/plugin_installer.py")
set(world_file "default_world.tcworld")
set(world_dest_dir "${dev_conf_dir}/server/default_world")
file(MAKE_DIRECTORY ${world_dest_dir})
set(world_dest_file "${world_dest_dir}/${world_file}")
set(world_source_dir ${plugins_dest_dir})
file(GLOB_RECURSE world_source_files "${world_source_dir}/*")

add_custom_command(
    OUTPUT ${world_dest_file}
    COMMAND python2 "${plugin_installer}" --fullname "Dev World" --identifier "dev" --recreate "${world_dest_file}" "${world_source_dir}"
    DEPENDS ${plugin_installer} ${world_source_files} ${sdk_files}
)
add_custom_target(dev_world ALL DEPENDS ${plugin_installer} ${world_source_files} ${sdk_files} ${world_dest_file})
add_dependencies(dev_world base_plugin)
