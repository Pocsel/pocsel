#ifndef __SERVER_GAME_ENGINE_EVENTMANAGER_HPP__
#define __SERVER_GAME_ENGINE_EVENTMANAGER_HPP__

namespace Tools {

    class SimpleMessageQueue;

}

namespace Server { namespace Game { namespace Engine {

    class EventManager
    {
    private:
        Tools::SimpleMessageQueue& _messageQueue;

    public:
        EventManager(Tools::SimpleMessageQueue& messageQueue);
        ~EventManager();
    };

}}}

#endif

