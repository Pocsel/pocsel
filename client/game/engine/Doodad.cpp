#include "client/game/engine/Doodad.hpp"
#include "tools/lua/Interpreter.hpp"
#include "bullet/bullet-all.hpp"

namespace Client { namespace Game { namespace Engine {

    Doodad::Doodad(Tools::Lua::Interpreter& interpreter, Uint32 id, Common::Physics::Node const& physics, DoodadType const& type) :
        _type(type),
        _self(interpreter.MakeTable()),
        _physics(physics)
    {
        this->_self.Set("id", id);
        Tools::debug << "Doodad::Doodad: New doodad \"" << this->_type.GetName() << "\" spawned (id: " << id << ", pos: " << this->_physics.position.r.x << " " << this->_physics.position.r.y << " " << this->_physics.position.r.z << ")" << std::endl;

        static btCollisionShape* colShape = new btBoxShape(btVector3(1, 2, 1));

        /// Create Dynamic Objects
        btTransform startTransform;
        startTransform.setIdentity();

        btScalar mass(12);
        btVector3 localInertia(0, 0, 0);

        colShape->calculateLocalInertia(mass, localInertia);

        startTransform.setOrigin(btVector3(
                    btScalar(physics.position.r.x),
                    btScalar(physics.position.r.y),
                    btScalar(physics.position.r.z)));

        //using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
        btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
        btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);
        _btBody = new btRigidBody(rbInfo);
    }

    Doodad::~Doodad()
    {
        Tools::debug << "Doodad::~Doodad: Destroying doodad \"" << this->_type.GetName() << "\" (pos: " << this->_physics.position.r.x << " " << this->_physics.position.r.y << " " << this->_physics.position.r.z << ")" << std::endl;
    }

}}}
