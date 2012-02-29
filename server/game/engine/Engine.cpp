#include "server/game/engine/Engine.hpp"
#include "server/game/engine/EventManager.hpp"
#include "server/game/engine/EntityManager.hpp"
#include "tools/lua/Interpreter.hpp"

namespace Server { namespace Game { namespace Engine {

    Engine::Engine()
    {
        Tools::debug << "Engine::Engine()\n";
        this->_interpreter = new Tools::Lua::Interpreter();
        this->_interpreter->Globals().Set("Server", this->_interpreter->MakeTable());
        this->_entityManager = new EntityManager(*this);
        this->_eventManager = new EventManager(*this);
    }

    Engine::~Engine()
    {
        Tools::debug << "Engine::~Engine()\n";
        Tools::Delete(this->_eventManager);
        Tools::Delete(this->_entityManager);
        Tools::Delete(this->_interpreter);
    }

    void Engine::Tick(Uint64 currentTime)
    {
        this->_currentTime = currentTime;
        this->_eventManager->DispatchEvents();
    }

}}}
