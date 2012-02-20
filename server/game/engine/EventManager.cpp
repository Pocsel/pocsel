#include "server/game/engine/EventManager.hpp"

#include "tools/SimpleMessageQueue.hpp"

namespace Server { namespace Game { namespace Engine {

    EventManager::EventManager(Tools::SimpleMessageQueue& messageQueue) :
        _messageQueue(messageQueue)
    {
        Tools::debug << "EventManager::EventManager()\n";
    }

    EventManager::~EventManager()
    {
        Tools::debug << "EventManager::~EventManager()\n";
    }

}}}

