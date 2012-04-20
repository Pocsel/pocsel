#include "server/game/engine/PositionalEntity.hpp"

namespace Server { namespace Game { namespace Engine {

    PositionalEntity::PositionalEntity(Engine& engine, Uint32 id, EntityType* type, Common::Position const& pos) :
        Entity(engine, id, type), _pos(pos)
    {
    }

}}}
