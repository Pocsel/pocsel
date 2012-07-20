#include "client/game/engine/BodyType.hpp"

namespace Client { namespace Game { namespace Engine {

    BodyType::BodyType(std::vector<ShapeNode> const& shapes) :
        Common::Physics::BodyType(shapes)
    {
        this->_DumpTree(_roots, "");
    }

}}}
