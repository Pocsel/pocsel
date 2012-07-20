#include "client/game/engine/Body.hpp"
#include "client/game/engine/BodyType.hpp"

namespace Client { namespace Game { namespace Engine {

    Body::Body(Common::Physics::BodyCluster& parent, BodyType const& bodyType) :
        Common::Physics::Body(parent, bodyType)
    {
    }

}}}

