#ifndef __SERVER_GAME_ENGINE_BODY_HPP__
#define __SERVER_GAME_ENGINE_BODY_HPP__

#include "common/physics/Body.hpp"

namespace Server { namespace Game { namespace Engine {

    class Engine;
    class BodyType;

    class Body :
        public Common::Physics::Body
    {
    public:
        Body(BodyType const& bodyType);
    };

}}}

#endif
