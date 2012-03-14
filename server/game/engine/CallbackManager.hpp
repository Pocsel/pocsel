#ifndef __SERVER_GAME_ENGINE_CALLBACKMANAGER_HPP__
#define __SERVER_GAME_ENGINE_CALLBACKMANAGER_HPP__

#include "tools/lua/Ref.hpp"

namespace Server { namespace Game { namespace Engine {

    class Engine;

    class CallbackManager :
        private boost::noncopyable
    {
    public:
        struct Callback
        {
            Callback(int targetId, std::string const& function, Tools::Lua::Ref const& arg, Tools::Lua::Ref const& bonusArg) :
                targetId(targetId), function(function), arg(arg), bonusArg(bonusArg)
            {
            }
            int targetId;
            std::string function;
            Tools::Lua::Ref arg;
            Tools::Lua::Ref bonusArg;
        };

    private:
        Engine& _engine;
        std::map<Uint32, Callback*> _callbacks;
        Uint32 _currentCallbackId;

    public:
        CallbackManager(Engine& engine);
        ~CallbackManager();
        Uint32 MakeCallback(int targetId, std::string const& function, Tools::Lua::Ref const& arg, Tools::Lua::Ref const& bonusArg);
        bool TriggerCallback(Uint32 callbackId); // retourne true si callbackId est effectivement associé a une callback
        bool CancelCallback(Uint32 callbackId); // idem
    };

}}}

#endif
