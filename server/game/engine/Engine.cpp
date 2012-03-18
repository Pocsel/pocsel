#include "server/game/engine/Engine.hpp"
#include "server/game/engine/CallManager.hpp"
#include "server/game/engine/EntityManager.hpp"
#include "server/game/engine/CallbackManager.hpp"
#include "tools/lua/Interpreter.hpp"

namespace Server { namespace Game { namespace Engine {

    Engine::Engine(Map::Map& map) :
        _map(map), _currentTime(0)
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
        this->_callManager = new CallManager(*this);
    }

    Engine::~Engine()
    {
        Tools::debug << "Engine::~Engine()\n";
        Tools::Delete(this->_callManager);
        Tools::Delete(this->_entityManager);
        Tools::Delete(this->_callbackManager);
        Tools::Delete(this->_interpreter);
    }

    void Engine::Tick(Uint64 currentTime)
    {
        this->_currentTime = currentTime;
        this->_entityManager->DispatchKillEvents();
        this->_entityManager->DispatchSpawnEvents();
        this->_callManager->DispatchCalls();
    }

    void Engine::Save(Tools::Database::IConnection& conn)
    {
        this->_entityManager->Save(conn);
    }

}}}
