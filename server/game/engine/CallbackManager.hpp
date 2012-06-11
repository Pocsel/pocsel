#ifndef __SERVER_GAME_ENGINE_CALLBACKMANAGER_HPP__
#define __SERVER_GAME_ENGINE_CALLBACKMANAGER_HPP__

#include "tools/lua/Ref.hpp"

namespace Tools { namespace Database {
    class IConnection;
}}

namespace Server { namespace Game { namespace Engine {

    class Engine;

    class CallbackManager :
        private boost::noncopyable
    {
    public:
        struct Callback
        {
            Callback(Uint32 entityId, std::string const& function, Tools::Lua::Ref const& arg) :
                entityId(entityId), function(function), arg(arg)
            {
            }
            Uint32 entityId;
            std::string function;
            Tools::Lua::Ref arg;
        };
        enum Result
        {
            EntityNotFound, // le entityId de la callback ne pointe pas vers une entité chargée
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
        Uint32 MakeCallback(Uint32 entityId, std::string const& function, Tools::Lua::Ref const& arg, bool serialize = true);
        Result TriggerCallback(Uint32 callbackId, Tools::Lua::Ref* ret = 0, bool keepCallback = false);
        Result TriggerCallback(Uint32 callbackId, Tools::Lua::Ref const& bonusArg, Tools::Lua::Ref* ret = 0, bool keepCallback = false);
        bool CancelCallback(Uint32 callbackId); // retourne true si callbackId est effectivement associé a une callback (annulation réussie)
        Callback const& GetCallback(Uint32 callbackId) const throw(std::runtime_error); // ne pas garder de reference, la callback peut etre delete à tout moment
        void Save(Tools::Database::IConnection& conn);
        void Load(Tools::Database::IConnection& conn);
    };

}}}

#endif
