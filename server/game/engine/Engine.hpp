#ifndef __SERVER_GAME_ENGINE_ENGINE_HPP__
#define __SERVER_GAME_ENGINE_ENGINE_HPP__

namespace Tools {

    class SimpleMessageQueue;

}

namespace Server { namespace Game { namespace Engine {

    class EventManager;

    class Engine
    {
    private:
        Tools::SimpleMessageQueue& _messageQueue;
        EventManager* _eventManager;

    public:
        Engine(Tools::SimpleMessageQueue& messageQueue);
        ~Engine();
    };

}}}

#endif
