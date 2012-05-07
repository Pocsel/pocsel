#ifndef __TOOLS_PLUGIN_INSTALL_INSTALL_HPP__
#define __TOOLS_PLUGIN_INSTALL_INSTALL_HPP__

namespace Tools { namespace PluginInstall {

    bool Install(boost::filesystem::path const& pluginFile, boost::filesystem::path const& worldFile);

    bool Uninstall(std::string const& pluginIdentifier, boost::filesystem::path const& worldFile);

}}

#endif
