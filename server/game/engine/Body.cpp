#include "server/game/engine/Body.hpp"
#include "server/game/engine/BodyType.hpp"


namespace Server { namespace Game { namespace Engine {

    Body::Body(Common::Physics::BodyCluster& parent, BodyType const& bodyType) :
        Common::Physics::Body(parent, bodyType)
    {
    }

}}}
