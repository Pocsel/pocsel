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
            Callback(Uint32 targetId, std::string const& function, Tools::Lua::Ref const& arg) :
                targetId(targetId), function(function), arg(arg)
            {
            }
            Uint32 targetId;
            std::string function;
            Tools::Lua::Ref arg;
        };
        enum Result
        {
            EntityNotFound, // le targetId de la callback ne pointe pas vers une entité chargée
            CallbackNotFound, // callbackId invalide
            FunctionNotFound, // l'entité a été trouvée mais pas la fonction (rien ne se passe)
            Error, // l'appel a été effectué mais l'entité a throw (l'entité est supprimée)
            Ok, // l'appel a été effectué
        };

    private:
        Engine& _engine;
        std::map<Uint32, Callback*> _callbacks;
        Uint32 _nextCallbackId;

    public:
        CallbackManager(Engine& engine);
        ~CallbackManager();
        Uint32 MakeCallback(Uint32 targetId, std::string const& function, Tools::Lua::Ref const& arg, bool serialize = true);
        Result TriggerCallback(Uint32 callbackId, bool keepCallback = false);
        Result TriggerCallback(Uint32 callbackId, Tools::Lua::Ref const& bonusArg, bool keepCallback = false);
        bool CancelCallback(Uint32 callbackId); // retourne true si callbackId est effectivement associé a une callback (annulation réussie)
    };

}}}

#endif
