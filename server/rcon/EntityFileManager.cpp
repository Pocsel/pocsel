#include "server/rcon/EntityFileManager.hpp"
#include "server/rcon/ToJsonStr.hpp"
#include "server/Server.hpp"
#include "server/game/Game.hpp"
#include "server/game/World.hpp"
#include "server/game/PluginManager.hpp"

namespace Server { namespace Rcon {

    EntityFileManager::EntityFileManager(Server& server) :
        _server(server)
    {
    }

    void EntityFileManager::AddFile(Uint32 pluginId, std::string const& file, std::string const& lua)
    {
        EntityFile f;
        f.pluginId = pluginId;
        f.file = file;
        f.lua = lua;
        this->_files.push_back(f);
    }

    void EntityFileManager::UpdateFile(std::string const& pluginIdentifier, std::string const& file, std::string const& lua)
    {
    }

    std::string EntityFileManager::GetFile(std::string const& pluginIdentifier, std::string const& file) const
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

    std::string EntityFileManager::RconGetEntityFiles() const
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
                "\t\t\t\"file\": \"" + ToJsonStr(it->file) + "\"\n" + // ptetre bien que JsonToStr n'est pas necessaire ici
                "\t\t}";
        }
        json += "\n\t]\n";
        return json;
    }

}}
