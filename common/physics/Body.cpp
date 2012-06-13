#include "common/physics/Body.hpp"
#include "common/physics/BodyType.hpp"


namespace Common { namespace Physics {

    Body::Body(BodyType const& bodyType) :
        _type(bodyType),
        _nodes(bodyType.GetShapes().size())
    {
    }

}}
