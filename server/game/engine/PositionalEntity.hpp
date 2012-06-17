#ifndef __SERVER_GAME_ENGINE_POSITIONALENTITY_HPP__
#define __SERVER_GAME_ENGINE_POSITIONALENTITY_HPP__

#include "common/physics/Node.hpp"
#include "server/game/engine/Entity.hpp"

class btRigidBody;

namespace Server { namespace Game { namespace Engine {

    class PositionalEntity :
        public Entity
    {
    private:
        Common::Physics::Node _physics;
        btRigidBody* _btBody;

    public:
        PositionalEntity(Tools::Lua::Interpreter& interpreter, Uint32 id, EntityType const& type, Common::Position const& pos);
        ~PositionalEntity();

        Common::Position const& GetPosition() const { return this->_physics.position.r; }
        void SetPosition(Common::Position const& pos) { this->_physics.position.r = pos; }
        glm::dvec3 const& GetSpeed() const { return this->_physics.position.v; }
        void SetSpeed(glm::dvec3 const& speed) { this->_physics.position.v = speed; }
        glm::dvec3 const& GetAccel() const { return this->_physics.position.a; }
        void SetAccel(glm::dvec3 const& accel) { this->_physics.position.a = accel; }

        Common::Physics::Node const& GetPhysics() const { return this->_physics; }
        void SetPhysics(Common::Physics::Node const& p) { this->_physics = p; }

        Common::Physics::Node& GetPhysics() { return this->_physics; }
        btRigidBody& GetBtBody() { return *this->_btBody; }
    };

}}}

#endif
