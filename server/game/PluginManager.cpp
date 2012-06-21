#include "server/game/PluginManager.hpp"
#include "server/rcon/ToJsonStr.hpp"
#include "common/FieldUtils.hpp"

namespace Server { namespace Game {

    void PluginManager::AddPlugin(Uint32 id, std::string const& fullname, std::string const& identifier) throw(std::runtime_error)
    {
        Plugin p;
        if (!Common::FieldUtils::IsPluginFullname(p.fullname = fullname))
            throw std::runtime_error("PluginManager::AddPlugin: Invalid plugin fullname \"" + fullname + "\".");
        if (!Common::FieldUtils::IsPluginIdentifier(p.identifier = identifier))
            throw std::runtime_error("PluginManager::AddPlugin: Invalid plugin identifier \"" + identifier + "\".");
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

    Uint32 PluginManager::GetPluginId(std::string const& identifier) const
    {
        auto it = this->_plugins.begin();
        auto itEnd = this->_plugins.end();
        for (; it != itEnd; ++it)
            if (it->second.identifier == identifier)
                return it->first;
        return 0;
    }

    std::string PluginManager::RconGetPlugins() const
    {
        std::string json = "\t[\n";
        auto it = this->_plugins.begin();
        auto itEnd = this->_plugins.end();
        for (; it != itEnd; ++it)
        {
            if (it != this->_plugins.begin())
                json += ",\n";
            json += "\t\t{\n"
                "\t\t\t\"identifier\": \"" + it->second.identifier + "\",\n"
                "\t\t\t\"fullname\": \"" + Rcon::ToJsonStr(it->second.fullname) + "\"\n"
                "\t\t}";
        }
        json += "\n\t]\n";
        return json;
    }

}}
