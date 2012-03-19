#ifndef __SERVER_GAME_ENGINE_MESSAGEMANAGER_HPP__
#define __SERVER_GAME_ENGINE_MESSAGEMANAGER_HPP__

#include "tools/lua/Ref.hpp"

namespace Server { namespace Game { namespace Engine {

    class Engine;

    class MessageManager :
        private boost::noncopyable
    {
    private:
        struct Message
        {
            Message(Uint32 targetId, Uint32 callbackId, Uint32 notificationCallbackId) :
                targetId(targetId), callbackId(callbackId), notificationCallbackId(notificationCallbackId)
            {
            }
            Uint32 targetId;
            Uint32 callbackId;
            Uint32 notificationCallbackId;
        };

    private:
        Engine& _engine;
        std::map<Uint64, std::list<Message*>> _messages;

    public:
        MessageManager(Engine& engine);
        ~MessageManager();
        void DispatchMessages();
    private:
        void _ApiLater(Tools::Lua::CallHelper& helper);
        void _ApiNow(Tools::Lua::CallHelper& helper);
    };

}}}

#endif

