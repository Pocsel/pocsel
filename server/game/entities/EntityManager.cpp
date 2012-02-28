#include "server/game/entities/EntityManager.hpp"

#include "tools/SimpleMessageQueue.hpp"

namespace Server { namespace Game { namespace Entities {

    EntityManager::EntityManager(Tools::SimpleMessageQueue& messageQueue) :
        _messageQueue(messageQueue)
    {
        Tools::debug << "EntityManager::EntityManager()\n";
    }

    EntityManager::~EntityManager()
    {
        Tools::debug << "EntityManager::~EntityManager()\n";
    }

    void EntityManager::Start()
    {
        Tools::debug << "EntityManager::Start()\n";
    }

    void EntityManager::Stop()
    {
        Tools::debug << "EntityManager::Stop()\n";
    }

}}}
