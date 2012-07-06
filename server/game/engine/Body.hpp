#ifndef __SERVER_GAME_ENGINE_BODY_HPP__
#define __SERVER_GAME_ENGINE_BODY_HPP__

#include "common/physics/Body.hpp"
#include "server/game/engine/BodyType.hpp"

namespace Common { namespace Physics {
    class World;
}}

namespace Server { namespace Game { namespace Engine {

    class Engine;

    class Body :
        public Common::Physics::Body
    {
    public:
        Body(Common::Physics::BodyCluster& parent, BodyType const& bodyType);
        BodyType const& GetType() const
        {
            return reinterpret_cast<::Server::Game::Engine::BodyType const&>(this->_type);
        }
    };

}}}

#endif
