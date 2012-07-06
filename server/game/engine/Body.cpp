#include "server/game/engine/Body.hpp"
#include "server/game/engine/BodyType.hpp"


namespace Server { namespace Game { namespace Engine {

    Body::Body(Common::Physics::World& world, Common::Physics::Node const& position, BodyType const& bodyType) :
        Common::Physics::Body(world, position, bodyType)
    {
    }

}}}
