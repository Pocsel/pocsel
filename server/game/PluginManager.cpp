#include "server/game/PluginManager.hpp"

namespace Server { namespace Game {

    PluginManager::PluginManager()
    {
    }

    void PluginManager::AddPlugin(Uint32 id, std::string const& fullname, std::string const& identifier) throw(std::runtime_error)
    {
        Plugin p;
        p.fullname = fullname;
        p.identifier = identifier;
        if (this->_plugins.count(id) > 0)
            throw std::runtime_error("PluginManager::AddPlugin: Plugin id already used");
        this->_plugins[id] = p;
    }

    PluginManager::Plugin const& PluginManager::GetPlugin(Uint32 id) const throw(std::runtime_error)
    {
        auto it = this->_plugins.find(id);
        if (it != this->_plugins.end())
            return it->second;
        throw std::runtime_error("PluginManager::GetPlugin: Plugin not found");
    }

    std::string PluginManager::GetPluginFullname(Uint32 id) const
    {
        auto it = this->_plugins.find(id);
        if (it != this->_plugins.end())
            return it->second.fullname;
        return "(plugin not found)";
    }

    std::string PluginManager::GetPluginIdentifier(Uint32 id) const
    {
        auto it = this->_plugins.find(id);
        if (it != this->_plugins.end())
            return it->second.identifier;
        return "(plugin not found)";
    }

}}
