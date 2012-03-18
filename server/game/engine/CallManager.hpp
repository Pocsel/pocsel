#ifndef __SERVER_GAME_ENGINE_CALLMANAGER_HPP__
#define __SERVER_GAME_ENGINE_CALLMANAGER_HPP__

#include "tools/lua/Ref.hpp"

namespace Server { namespace Game { namespace Engine {

    class Engine;

    class CallManager :
        private boost::noncopyable
    {
    private:
        struct Call
        {
            Call(Uint32 targetId, Uint32 callbackId, Uint32 notificationCallbackId) :
                targetId(targetId), callbackId(callbackId), notificationCallbackId(notificationCallbackId)
            {
            }
            Uint32 targetId;
            Uint32 callbackId;
            Uint32 notificationCallbackId;
        };

    private:
        Engine& _engine;
        std::map<Uint64, std::list<Call*>> _calls;

    public:
        CallManager(Engine& engine);
        ~CallManager();
        void DispatchCalls();
    private:
        void _ApiLater(Tools::Lua::CallHelper& helper);
        void _ApiNow(Tools::Lua::CallHelper& helper);
    };

}}}

#endif

