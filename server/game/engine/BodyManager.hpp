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
        std::vector<BodyType*> _bodyTypesVec;

    public:
        BodyManager(Engine& engine);
        ~BodyManager();

        Body* CreateBody(Uint32 pluginId, std::string BodyName) throw(std::runtime_error);
        // rcon requests
        //std::string RconGetBodies() const;
        //void RconAddBodyTypes(Rcon::BodyManager& manager) const; // pas vraiment une requete, mais c'est bien spécifique à Rcon

        bool HasBodyType(Uint32 id) const { return id && id <= this->_bodyTypesVec.size(); }
        BodyType const& GetBodyType(Uint32 id) const { assert(this->HasBodyType(id)); return *this->_bodyTypesVec[id - 1]; }
        Uint32 GetNbBodyTypes() const { return (Uint32)this->_bodyTypesVec.size(); }

    private:
        void _ApiRegister(Tools::Lua::CallHelper& helper);
    };

}}}

#endif

