#include "server/game/entities/EntityManager.hpp"

#include "tools/SimpleMessageQueue.hpp"

namespace Server { namespace Game { namespace Entities {

    EntityManager::EntityManager() :
        _messageQueue(new Tools::SimpleMessageQueue(1))
    {
        Tools::debug << "EntityManager::EntityManager()\n";
    }

    EntityManager::~EntityManager()
    {
        Tools::debug << "EntityManager::~EntityManager()\n";
        Tools::Delete(this->_messageQueue);
    }

    void EntityManager::Start()
    {
        Tools::debug << "EntityManager::Start()\n";
        this->_messageQueue->Start();
    }

    void EntityManager::Stop()
    {
        Tools::debug << "EntityManager::Stop()\n";
        this->_messageQueue->Stop();
    }

}}}
