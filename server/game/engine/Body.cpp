#include "server/game/engine/Body.hpp"
#include "server/game/engine/BodyType.hpp"


namespace Server { namespace Game { namespace Engine {

    Body::Body(BodyType const& bodyType) :
        Tools::Physics::Body(bodyType)
    {
    }

}}}
