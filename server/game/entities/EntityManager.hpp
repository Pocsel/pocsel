#ifndef __SERVER_GAME_ENTITIES_ENTITYMANAGER_HPP__
#define __SERVER_GAME_ENTITIES_ENTITYMANAGER_HPP__

namespace Tools {

    class SimpleMessageQueue;

}

namespace Server { namespace Game { namespace Entities {

    class EntityManager
    {
    private:
        Tools::SimpleMessageQueue& _messageQueue;

    public:
        EntityManager(Tools::SimpleMessageQueue& messageQueue);
        ~EntityManager();

        void Start();
        void Stop();
    };

}}}

#endif
