#ifndef __TOOLS_PLUGIN_INSTALL_INSTALL_HPP__
#define __TOOLS_PLUGIN_INSTALL_INSTALL_HPP__

namespace Tools { namespace PluginInstall {

    // si identifier ou fullname vide, ne crée pas de world si il n'existe pas (retourne false)
    // si le world existe deja, identifier et fullname sont ignorés
    bool Install(boost::filesystem::path const& pluginFile, boost::filesystem::path const& worldFile, std::string const& identifier = "", std::string const& fullname = "");

    bool Uninstall(std::string const& pluginIdentifier, boost::filesystem::path const& worldFile);

}}

#endif
