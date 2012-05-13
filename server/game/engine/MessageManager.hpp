#ifndef __SERVER_GAME_ENGINE_MESSAGEMANAGER_HPP__
#define __SERVER_GAME_ENGINE_MESSAGEMANAGER_HPP__

#include "tools/lua/Ref.hpp"

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

        // rcon requests
        std::string RconGetMessages() const;

    private:
        void _ApiLater(Tools::Lua::CallHelper& helper);
        void _ApiNow(Tools::Lua::CallHelper& helper);
        void _Load(Tools::Database::IConnection& conn);
    };

}}}

#endif
