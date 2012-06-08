#ifndef __CLIENT_GAME_ENGINE_BODYTYPE_HPP__
#define __CLIENT_GAME_ENGINE_BODYTYPE_HPP__

#include "tools/physics/BodyType.hpp"

namespace Client { namespace Game { namespace Engine {

    class BodyType :
        public Tools::Physics::BodyType
    {
    public:
        //BodyType() {}
        BodyType(std::vector<ShapeNode> const& shapes) :
            Tools::Physics::BodyType(shapes)
        {
        }
    };

}}}

#endif

