#ifndef __SERVER_GAME_ENGINE_POSITIONALENTITY_HPP__
#define __SERVER_GAME_ENGINE_POSITIONALENTITY_HPP__

#include "common/Position.hpp"
#include "server/game/engine/Entity.hpp"

namespace Server { namespace Game { namespace Engine {

    class PositionalEntity :
        public Entity
    {
    private:
        Common::Position _pos;

    public:
        PositionalEntity(Engine& engine, Uint32 id, EntityType* type, Common::Position const& pos);
        Common::Position const& GetPosition() const { return this->_pos; }
    };

}}}

#endif
