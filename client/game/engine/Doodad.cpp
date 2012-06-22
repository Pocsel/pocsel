#include "client/game/engine/Doodad.hpp"
#include "tools/lua/Interpreter.hpp"
#include "bullet/bullet-all.hpp"

namespace Client { namespace Game { namespace Engine {

    Doodad::Doodad(Tools::Lua::Interpreter& interpreter, Uint32 id, Common::Physics::Node const& physics, DoodadType const& type) :
        _type(type),
        _self(interpreter.MakeTable()),
        _physics(physics),
        _updateFlag(0.0f)
    {
        this->_self.Set("id", id);
        Tools::debug << "Doodad::Doodad: New doodad \"" << this->_type.GetName() << "\" spawned (id: " << id << ", pos: " << this->_physics.position.x << " " << this->_physics.position.y << " " << this->_physics.position.z << ")" << std::endl;

        static btCollisionShape* colShape = new
            //btSphereShape(2) // on donne le rayon
            btBoxShape(btVector3(0.1, 4, 2)) // on donne la moitié de la taille
            ;

        /// Create Dynamic Objects

        btScalar mass(1200);
        btVector3 localInertia(0, 0, 0);

        colShape->calculateLocalInertia(mass, localInertia);

        btTransform startTransform;
        startTransform.setIdentity();
        startTransform.setOrigin(btVector3(
                    btScalar(physics.position.x),
                    btScalar(physics.position.y),
                    btScalar(physics.position.z)));

        //using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
        btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
        myMotionState->setWorldTransform(startTransform);
        btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);
        _btBody = new btRigidBody(rbInfo);
    }

    Doodad::~Doodad()
    {
        Tools::debug << "Doodad::~Doodad: Destroying doodad \"" << this->_type.GetName() << "\" (pos: " << this->_physics.position.x << " " << this->_physics.position.y << " " << this->_physics.position.z << ")" << std::endl;
    }

}}}
