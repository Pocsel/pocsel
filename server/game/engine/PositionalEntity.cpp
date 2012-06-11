#include "server/game/engine/PositionalEntity.hpp"

namespace Server { namespace Game { namespace Engine {

    PositionalEntity::PositionalEntity(Tools::Lua::Interpreter& interpreter, Uint32 id, EntityType const& type, Common::Position const& pos) :
        Entity(interpreter, id, type)
    {
        _pos.position.r = pos;
    }

}}}
