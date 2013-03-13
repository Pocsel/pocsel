#ifndef __SERVER_GAME_ENGINE_MESSAGEMANAGER_HPP__
#define __SERVER_GAME_ENGINE_MESSAGEMANAGER_HPP__

#include <luasel/Luasel.hpp>

namespace Tools { namespace Database {
    class IConnection;
}}

namespace Server { namespace Game { namespace Engine {

    class Engine;

    class MessageManager :
        private boost::noncopyable
    {
    private:
        struct Message
        {
            Message(Uint32 callbackId, Uint32 notificationCallbackId) :
                callbackId(callbackId), notificationCallbackId(notificationCallbackId)
            {
            }
            Uint32 callbackId;
            Uint32 notificationCallbackId;
        };

    private:
        Engine& _engine;
        std::map<Uint64 /* time */, std::list<Message*>> _messages;

    public:
        MessageManager(Engine& engine);
        ~MessageManager();
        void DispatchMessages();
        void Save(Tools::Database::IConnection& conn);
        void Load(Tools::Database::IConnection& conn);

        // rcon requests
        std::string RconGetMessages() const;

    private:
        void _ApiLater(Luasel::CallHelper& helper);
        void _ApiNow(Luasel::CallHelper& helper);
    };

}}}

#endif
