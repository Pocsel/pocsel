#include "server/game/engine/Body.hpp"
#include "server/game/engine/BodyType.hpp"


namespace Tools { namespace Physics {

    Body::Body(BodyType const& bodyType) :
        _type(bodyType),
        _nodes(bodyType.GetShapes().size())
    {
    }

}}
