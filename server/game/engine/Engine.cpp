#include "server/game/engine/Engine.hpp"
#include "server/game/engine/EventManager.hpp"

#include "tools/SimpleMessageQueue.hpp"

namespace Server { namespace Game { namespace Engine {

    Engine::Engine(Tools::SimpleMessageQueue& messageQueue) :
        _messageQueue(messageQueue)
    {
        Tools::debug << "Engine::Engine()\n";
        this->_eventManager = new EventManager(messageQueue);
    }

    Engine::~Engine()
    {
        Tools::debug << "Engine::~Engine()\n";
        Tools::Delete(this->_eventManager);
    }

}}}
