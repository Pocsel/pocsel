#ifndef __SERVER_GAME_ENGINE_BODYMANAGER_HPP__
#define __SERVER_GAME_ENGINE_BODYMANAGER_HPP__

#include "tools/lua/Ref.hpp"
#include "server/game/engine/CallbackManager.hpp"
#include "common/Position.hpp"

namespace Server { namespace Game { namespace Engine {

    class Engine;
    class Body;
    class BodyType;

    class BodyManager :
        private boost::noncopyable
    {
    private:
        Engine& _engine;
        std::map<Uint32 /* pluginId */, std::map<std::string /* BodyName */, BodyType*>> _bodyTypes;

    public:
        BodyManager(Engine& engine);
        ~BodyManager();

        Body* CreateBody(Uint32 pluginId, std::string BodyName) throw(std::runtime_error);
        // rcon requests
        //std::string RconGetBodies() const;
        //void RconAddBodyTypes(Rcon::BodyManager& manager) const; // pas vraiment une requete, mais c'est bien spécifique à Rcon

    private:
        void _ApiRegister(Tools::Lua::CallHelper& helper);
    };

}}}

#endif

