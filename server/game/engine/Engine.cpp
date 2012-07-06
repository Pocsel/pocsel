#include "server/game/engine/Engine.hpp"
#include "server/game/engine/MessageManager.hpp"
#include "server/game/engine/EntityManager.hpp"
#include "server/game/engine/CallbackManager.hpp"
#include "server/game/engine/DoodadManager.hpp"
#include "server/game/engine/PhysicsManager.hpp"
#include "server/game/engine/BodyManager.hpp"
#include "server/game/engine/Entity.hpp"
#include "server/game/engine/EntityType.hpp"
#include "server/game/World.hpp"
#include "server/game/map/Map.hpp"
#include "server/game/PluginManager.hpp"
#include "server/rcon/ToJsonStr.hpp"
#include "tools/lua/Interpreter.hpp"
#include "tools/lua/utils/Utils.hpp"
#include "tools/database/IConnection.hpp"
#include "common/FieldUtils.hpp"

namespace Server { namespace Game { namespace Engine {

    Engine::Engine(Map::Map& map, World& world) :
        _map(map), _world(world), _currentTime(0), _overriddenPluginId(0), _overriddenEntityId(0)
    {
        Tools::debug << "Engine::Engine()\n";
        this->_interpreter = new Tools::Lua::Interpreter();
        this->_interpreter->RegisterLib(Tools::Lua::Interpreter::Base);
        this->_interpreter->Globals().Set("print", this->_interpreter->MakeFunction(std::bind(&Engine::_ApiPrint, this, std::placeholders::_1)));
        this->_interpreter->RegisterLib(Tools::Lua::Interpreter::Math);
        this->_interpreter->RegisterLib(Tools::Lua::Interpreter::Table);
        this->_interpreter->RegisterLib(Tools::Lua::Interpreter::String);
        Tools::Lua::Utils::RegisterVector(*this->_interpreter);
        auto namespaceTable = this->_interpreter->Globals().Set("Server", this->_interpreter->MakeTable());
        this->_callbackManager = new CallbackManager(*this);
        this->_entityManager = new EntityManager(*this);
        this->_messageManager = new MessageManager(*this);
        this->_doodadManager = new DoodadManager(*this);
        this->_physicsManager = new PhysicsManager(*this, this->_entityManager->GetPositionalEntities());
        this->_bodyManager = new BodyManager(*this);
    }

    Engine::~Engine()
    {
        Tools::debug << "Engine::~Engine()\n";
        Tools::Delete(this->_bodyManager);
        Tools::Delete(this->_doodadManager);
        Tools::Delete(this->_messageManager);
        Tools::Delete(this->_entityManager);
        Tools::Delete(this->_physicsManager);
        Tools::Delete(this->_callbackManager);
        Tools::Delete(this->_interpreter);
    }

    void Engine::Tick(Uint64 currentTime)
    {
        Uint64 deltaTime = currentTime - this->_currentTime;
        this->_currentTime = currentTime;
        this->_entityManager->DispatchKillEvents();
        this->_entityManager->DispatchSpawnEvents();
        this->_physicsManager->Tick(deltaTime);
        this->_messageManager->DispatchMessages();
        this->_doodadManager->ExecuteCommands(); // PS: l'ordre a une importance de ouf
    }

    void Engine::Load(Tools::Database::IConnection& conn)
    {
        // load managers
        this->_callbackManager->Load(conn);
        this->_messageManager->Load(conn);
        this->_entityManager->Load(conn);
        this->_doodadManager->Load(conn); // PS: l'ordre a une importance de ouf
        // initializes any plugin that needs it
        auto const& plugins = this->_world.GetPluginManager().GetPlugins();
        auto it = plugins.begin();
        auto itEnd = plugins.end();
        for (; it != itEnd; ++it)
            this->_entityManager->BootstrapPlugin(it->first, conn);
    }

    void Engine::Save(Tools::Database::IConnection& conn)
    {
        // save managers
        this->_callbackManager->Save(conn);
        this->_messageManager->Save(conn);
        this->_entityManager->Save(conn);
        this->_doodadManager->Save(conn); // PS: l'ordre a une importance de ouf
        // mark all plugins as initialized
        std::string table = this->_map.GetName() + "_initialized_plugin";
        conn.CreateQuery("DELETE FROM " + table + ";")->ExecuteNonSelect();
        auto query = conn.CreateQuery("INSERT INTO " + table + " (id) VALUES (?);");
        auto const& plugins = this->_world.GetPluginManager().GetPlugins();
        auto it = plugins.begin();
        auto itEnd = plugins.end();
        for (; it != itEnd; ++it)
            try
            {
                Tools::debug << ">> Save >> " << table << " >> Initialized Plugin (pluginId: " << it->first << ")" << std::endl;
                query->Bind(it->first).ExecuteNonSelect().Reset();
            }
            catch (std::exception& e)
            {
                Tools::error << "Engine::Save: Could not mark plugin " << it->first << " as initialized: " << e.what() << std::endl;
            }
    }

    void Engine::SendPacket(Uint32 playerId, std::unique_ptr<Common::Packet>& packet)
    {
        this->_map.SendPacket(playerId, packet);
    }

    void Engine::SendUdpPacket(Uint32 playerId, std::unique_ptr<Network::UdpPacket>& packet)
    {
        this->_map.SendUdpPacket(playerId, packet);
    }

    // uniquement en debug (pour pouvoir require() pour faire du hot-swap lua)
    void Engine::SetModules(std::map<Uint32 /* pluginId */, std::map<std::string /* server_file name */, std::pair<bool /* loading in progress */, Tools::Lua::Ref /* module */>>> const& modules)
    {
        auto itPlugin = modules.begin();
        auto itPluginEnd = modules.end();
        for (; itPlugin != itPluginEnd; ++itPlugin)
        {
            auto it = itPlugin->second.begin();
            auto itEnd = itPlugin->second.end();
            for (; it != itEnd; ++it)
                this->_modules[itPlugin->first].insert(std::make_pair(it->first, it->second.second));
        }
    }

    // uniquement en debug (pour pouvoir require() pour faire du hot-swap lua)
    void Engine::RegisterRequire()
    {
        this->_interpreter->Globals().Set("require", this->_interpreter->MakeFunction(std::bind(&Engine::_ApiRequire, this, std::placeholders::_1)));
    }

    // uniquement en debug (pour pouvoir require() pour faire du hot-swap lua)
    void Engine::_ApiRequire(Tools::Lua::CallHelper& helper)
    {
        std::string name = helper.PopArg("require: Missing argument \"name\"").CheckString("require: Argument \"name\" must be a string");
        Uint32 pluginId = this->_world.GetPluginManager().GetPluginId(Common::FieldUtils::GetPluginNameFromResource(name));
        std::string fileName = Common::FieldUtils::GetResourceNameFromResource(name);
        auto itPlugin = this->_modules.find(pluginId);
        if (itPlugin != this->_modules.end())
        {
            auto it = itPlugin->second.find(fileName);
            if (it != itPlugin->second.end())
                return helper.PushRet(it->second);
        }
        throw std::runtime_error("require: Server file \"" + fileName + "\" in plugin " + Tools::ToString(pluginId) + " not found");
    }

    void Engine::_ApiPrint(Tools::Lua::CallHelper& helper)
    {
        std::string str = "[" + this->_map.GetName() + "/";
        if (this->GetRunningPluginId())
            str += this->_world.GetPluginManager().GetPluginIdentifier(this->GetRunningPluginId());
        else
            str += "?";
        str += "/";
        try
        {
            auto& e = this->_entityManager->GetEntity(this->GetRunningEntityId());
            str += e.GetType().GetName() + "/" + Tools::ToString(this->GetRunningEntityId());
        }
        catch (std::exception&)
        {
            str += "?";
        }
        str += "] ";
        auto it = helper.GetArgList().begin();
        auto itEnd = helper.GetArgList().end();
        for (; it != itEnd; ++it)
            str += it->ToString();
        Tools::log << str << std::endl;
    }

    std::string Engine::RconExecute(Uint32 pluginId, std::string const& lua)
    {
        std::string json = "{\n"
            "\t\"log\": \"";
        this->OverrideRunningPluginId(pluginId);
        try
        {
            this->_interpreter->DoString(lua);
        }
        catch (std::exception& e)
        {
            Tools::error << "Engine::RconExecute: Error: " << e.what() << std::endl;
            json += Rcon::ToJsonStr(e.what());
        }
        this->OverrideRunningPluginId(0);
        json += "\"\n}\n";
        return json;
    }

}}}
