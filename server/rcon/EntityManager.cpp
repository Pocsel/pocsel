#include "server/rcon/EntityManager.hpp"
#include "server/rcon/ToJsonStr.hpp"
#include "server/Server.hpp"
#include "server/game/Game.hpp"
#include "server/game/World.hpp"
#include "server/game/PluginManager.hpp"

namespace Server { namespace Rcon {

    EntityManager::EntityManager(Server& server) :
        _server(server)
    {
    }

    void EntityManager::AddFile(Uint32 pluginId, std::string const& file, std::string const& lua)
    {
        EntityFile f;
        f.pluginId = pluginId;
        f.file = file;
        f.lua = lua;
        this->_files.push_back(f);
    }

    std::string EntityManager::GetFile(std::string const& pluginIdentifier, std::string const& file) const
    {
        Uint32 pluginId = this->_server.GetGame().GetWorld().GetPluginManager().GetPluginId(pluginIdentifier);
        auto it = this->_files.begin();
        auto itEnd = this->_files.end();
        for (; it != itEnd; ++it)
            if (it->pluginId == pluginId && it->file == file)
            {
                std::string json = "{\n";
                json += "\t\"lua\": \"" + ToJsonStr(it->lua) + "\"\n";
                json += "}\n";
                return json;
            }
        return std::string();
    }

    bool EntityManager::UpdateFile(std::string const& pluginIdentifier, std::string const& file, std::string const& lua)
    {
        Uint32 pluginId = this->_server.GetGame().GetWorld().GetPluginManager().GetPluginId(pluginIdentifier);
        auto it = this->_files.begin();
        auto itEnd = this->_files.end();
        for (; it != itEnd; ++it)
            if (it->pluginId == pluginId && it->file == file)
            {
                it->lua = lua;
                return true;
            }
        return false;
    }

    std::string EntityManager::RconGetEntityFiles() const
    {
        std::string json = "\t[\n";
        auto it = this->_files.begin();
        auto itEnd = this->_files.end();
        for (; it != itEnd; ++it)
        {
            if (it != this->_files.begin())
                json += ",\n";
            json +=
                "\t\t{\n"
                "\t\t\t\"plugin\": \"" + this->_server.GetGame().GetWorld().GetPluginManager().GetPluginIdentifier(it->pluginId) + "\",\n" +
                "\t\t\t\"file\": \"" + ToJsonStr(it->file) + "\"\n" + // ptetre bien que ToJsonStr n'est pas necessaire ici
                "\t\t}";
        }
        json += "\n\t]\n";
        return json;
    }

    void EntityManager::AddType(Uint32 pluginId, std::string const& name, bool positional)
    {
        EntityType t;
        t.pluginId = pluginId;
        t.name = name;
        t.positional = positional;
        this->_types.push_back(t);
    }

    std::string EntityManager::RconGetEntityTypes() const
    {
        std::string json = "\t[\n";
        auto it = this->_types.begin();
        auto itEnd = this->_types.end();
        for (; it != itEnd; ++it)
        {
            if (it != this->_types.begin())
                json += ",\n";
            json +=
                "\t\t{\n"
                "\t\t\t\"plugin\": \"" + this->_server.GetGame().GetWorld().GetPluginManager().GetPluginIdentifier(it->pluginId) + "\",\n" +
                "\t\t\t\"name\": \"" + it->name + "\",\n" +
                "\t\t\t\"positional\": " + (it->positional ? "true" : "false") + "\n" +
                "\t\t}";
        }
        json += "\n\t]\n";
        return json;
    }

}}
