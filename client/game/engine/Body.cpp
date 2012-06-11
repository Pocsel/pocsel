#include "client/game/engine/Body.hpp"
#include "client/game/engine/BodyType.hpp"

namespace Client { namespace Game { namespace Engine {

    Body::Body(BodyType const& bodyType) :
        Tools::Physics::Body(bodyType)
    {
    }

}}}

