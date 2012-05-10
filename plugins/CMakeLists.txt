# Plugins
# -------

set(plugin_creator "${EXECUTABLE_OUTPUT_PATH}/packager-cli")
set(plugins_dest_dir "${CMAKE_CURRENT_SOURCE_DIR}/build")

macro(MakePlugin target source_dir filename)
    file(GLOB_RECURSE plugin_source_files "${source_dir}/*")
    set(plugin_dest_file "${plugins_dest_dir}/${filename}")
    add_custom_command(
        OUTPUT ${plugin_dest_file}
        COMMAND packager-cli "${CMAKE_CURRENT_SOURCE_DIR}/${source_dir}" "${plugin_dest_file}"
        DEPENDS packager-cli ${plugin_source_files}
    )
    CreateGroups("${plugin_source_files}")
    add_custom_target(${target} ALL DEPENDS packager-cli ${plugin_source_files} ${plugin_dest_file} SOURCES ${plugin_source_files})
endmacro()

MakePlugin(base_plugin "base" "base.tcplug")

# Dev World
# ---------

set(plugin_installer "${EXECUTABLE_OUTPUT_PATH}/installer-cli")
set(world_file "default_world.tcworld")
set(world_dest_dir "${dev_conf_dir}/server/default_world")
file(MAKE_DIRECTORY ${world_dest_dir})
set(world_dest_file "${world_dest_dir}/${world_file}")
set(world_source_dir ${plugins_dest_dir})
file(GLOB_RECURSE world_source_files "${world_source_dir}/*")

add_custom_command(
    OUTPUT ${world_dest_file}
    COMMAND installer-cli "${world_source_dir}" "${world_dest_file}" "dev" "Dev World"
    DEPENDS installer-cli ${world_source_files}
)
CreateGroups("${world_source_files}")
add_custom_target(dev_world ALL DEPENDS installer-cli base_plugin ${world_source_files} ${world_dest_file} SOURCES ${world_source_files})
add_dependencies(dev_world base_plugin)
