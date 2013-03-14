#ifndef __CLIENT_GAME_ENGINE_BODY_HPP__
#define __CLIENT_GAME_ENGINE_BODY_HPP__

#include "common/physics/Body.hpp"
#include "client/game/engine/BodyType.hpp"

namespace Client { namespace Game { namespace Engine {

    class Engine;
    class BodyType;

    class Body :
        public Common::Physics::Body
    {
    private:
        std::vector<std::shared_ptr<Common::Physics::Node>> _boundNodes;

    public:
        Body(Common::Physics::BodyCluster& parent, BodyType const& bodyType);
        BodyType const& GetType() const
        {
            return reinterpret_cast< ::Client::Game::Engine::BodyType const&>(this->_type);
        }

        std::shared_ptr<Common::Physics::Node> BindNode(std::string const& nodeName);

    private:
        virtual void _PostTick();
    };

}}}

#endif
