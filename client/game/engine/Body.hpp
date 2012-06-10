#ifndef __CLIENT_GAME_ENGINE_BODY_HPP__
#define __CLIENT_GAME_ENGINE_BODY_HPP__

#include "tools/physics/Body.hpp"

namespace Client { namespace Game { namespace Engine {

    class Engine;
    class BodyType;

    class Body :
        public Tools::Physics::Body
    {
    public:
        Body(BodyType const& bodyType);
    };

}}}

#endif

