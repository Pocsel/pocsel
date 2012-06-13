#ifndef __CLIENT_GAME_ENGINE_BODYTYPE_HPP__
#define __CLIENT_GAME_ENGINE_BODYTYPE_HPP__

#include "common/physics/BodyType.hpp"

namespace Client { namespace Game { namespace Engine {

    class BodyType :
        public Common::Physics::BodyType
    {
    public:
        //BodyType() {}
        BodyType(std::vector<ShapeNode> const& shapes) :
            Common::Physics::BodyType(shapes)
        {
        }
    };

}}}

#endif

