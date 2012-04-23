#include "server/game/engine/Engine.hpp"
#include "server/game/engine/MessageManager.hpp"
#include "server/game/engine/EntityManager.hpp"
#include "server/game/engine/CallbackManager.hpp"
#include "server/rcon/ToJsonStr.hpp"
#include "tools/lua/Interpreter.hpp"

namespace Server { namespace Game { namespace Engine {

    Engine::Engine(Map::Map& map, World& world) :
        _map(map), _world(world), _currentTime(0), _overriddenPluginId(0), _overriddenEntityId(0)
    {
        Tools::debug << "Engine::Engine()\n";
        this->_interpreter = new Tools::Lua::Interpreter();
        this->_interpreter->RegisterLib(Tools::Lua::Interpreter::Base);
        this->_interpreter->RegisterLib(Tools::Lua::Interpreter::Math);
        this->_interpreter->RegisterLib(Tools::Lua::Interpreter::Table);
        this->_interpreter->RegisterLib(Tools::Lua::Interpreter::String);
        auto namespaceTable = this->_interpreter->Globals().Set("Server", this->_interpreter->MakeTable());
        this->_interpreter->Globals().Set("S", namespaceTable);
        this->_callbackManager = new CallbackManager(*this);
        this->_entityManager = new EntityManager(*this);
        this->_messageManager = new MessageManager(*this);
    }

    Engine::~Engine()
    {
        Tools::debug << "Engine::~Engine()\n";
        Tools::Delete(this->_messageManager);
        Tools::Delete(this->_entityManager);
        Tools::Delete(this->_callbackManager);
        Tools::Delete(this->_interpreter);
    }

    void Engine::Tick(Uint64 currentTime)
    {
        this->_currentTime = currentTime;
        this->_entityManager->DispatchKillEvents();
        this->_entityManager->DispatchSpawnEvents();
        this->_messageManager->DispatchMessages();
    }

    void Engine::Save(Tools::Database::IConnection& conn)
    {
        this->_entityManager->Save(conn);
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
