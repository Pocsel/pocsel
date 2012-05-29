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
        PositionalEntity(Tools::Lua::Interpreter& interpreter, Uint32 id, EntityType const& type, Common::Position const& pos);
        Common::Position const& GetPosition() const { return this->_pos; }
        void SetPosition(Common::Position const& pos) { this->_pos = pos; }
    };

}}}

#endif
