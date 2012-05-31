#include "server/game/engine/Body.hpp"

namespace Server { namespace Game { namespace Engine {

    Body::Body(Tools::Lua::Interpreter& interpreter, BodyType const& bodyType) :
        _type(bodyType)
    {
    }

}}}
