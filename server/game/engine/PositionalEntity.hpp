#ifndef __SERVER_GAME_ENGINE_POSITIONALENTITY_HPP__
#define __SERVER_GAME_ENGINE_POSITIONALENTITY_HPP__

#include "common/physics/Node.hpp"
#include "server/game/engine/Entity.hpp"

namespace Server { namespace Game { namespace Engine {

    class PositionalEntity :
        public Entity
    {
    private:
        Common::Physics::Node _physics;

    public:
        PositionalEntity(Tools::Lua::Interpreter& interpreter, Uint32 id, EntityType const& type, Common::Position const& pos);
        Common::Position const& GetPosition() const { return this->_physics.position.r; }
        void SetPosition(Common::Position const& pos) { this->_physics.position.r = pos; }
        Common::Physics::Node const& GetPhysics() const { return this->_physics; }
        void SetPhysics(Common::Physics::Node const& pos) { this->_physics = pos; }
    };

}}}

#endif
