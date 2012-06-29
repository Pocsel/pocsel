#ifndef __SERVER_GAME_ENGINE_POSITIONALENTITY_HPP__
#define __SERVER_GAME_ENGINE_POSITIONALENTITY_HPP__

#include "common/physics/Node.hpp"
#include "server/game/engine/Entity.hpp"

class btRigidBody;
class btTypedConstraint;
class btDefaultMotionState;

namespace Common { namespace Physics {
    class World;
}}

namespace Server { namespace Game { namespace Engine {

    class PositionalEntity :
        public Entity
    {
    private:
        Common::Physics::World& _world;
        Common::Physics::Node _physics;
        btRigidBody* _body;
        btDefaultMotionState* _motionState;
        std::map<btRigidBody const*, btTypedConstraint*> _doodadBodies;

    public:
        PositionalEntity(
                Tools::Lua::Interpreter& interpreter,
                Uint32 id,
                EntityType const& type,
                Common::Position const& pos,
                Common::Physics::World& world);
        ~PositionalEntity();

        Common::Position const& GetPosition() const { return this->_physics.position; }
        void SetPosition(Common::Position const& pos) { this->_physics.position = pos; }
        glm::dvec3 const& GetSpeed() const { return this->_physics.velocity; }
        void SetSpeed(glm::dvec3 const& speed) { this->_physics.velocity = speed; }
        glm::dvec3 const& GetAccel() const { return this->_physics.acceleration; }
        void SetAccel(glm::dvec3 const& accel) { this->_physics.acceleration = accel; }

        Common::Physics::Node const& GetPhysics() const { return this->_physics; }
        void SetPhysics(Common::Physics::Node const& p) { this->_physics = p; }

        Common::Physics::Node& GetPhysics() { return this->_physics; }

        //btRigidBody* GetBtBody() { return this->_body; }
        void AddConstraint(btRigidBody* doodadBody); // retourne son propre body
        void PopConstraint(btRigidBody const* doodadBody);
    };

}}}

#endif
