#include "server/game/engine/PositionalEntity.hpp"

#include "bullet/bullet-all.hpp"
#include "common/physics/World.hpp"

namespace Server { namespace Game { namespace Engine {

    PositionalEntity::PositionalEntity(Common::Physics::World& world, Tools::Lua::Interpreter& interpreter, Uint32 id, EntityType const& type, Common::Position const& pos) :
        Entity(interpreter, id, type),
        _world(world),
        _btBody(0),
        _motionState(0)
    {
        _physics.position = pos;

        static btCollisionShape* colShape = new
            // btSphereShape(2) // rayon
            btBoxShape(btVector3(0.6, 1.8, 0.8)) // on donne la moitié de la taille
            ;

        /// Create Dynamic Objects

        btScalar mass(60);
        btVector3 localInertia(0, 0, 0);

        colShape->calculateLocalInertia(mass, localInertia);

        btTransform startTransform;
        startTransform.setIdentity();
        startTransform.setOrigin(btVector3(
                    btScalar(pos.x),
                    btScalar(pos.y),
                    btScalar(pos.z)));

        //using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
        _motionState = new btDefaultMotionState(startTransform);
        //myMotionState->setWorldTransform(startTransform);
        btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, _motionState, colShape, localInertia);
        _btBody = new btRigidBody(rbInfo);

        _btBody->setActivationState(DISABLE_DEACTIVATION);

        _btBody->setUserPointer(this);

        _world.GetBtWorld().addRigidBody(_btBody);
    }

    PositionalEntity::~PositionalEntity()
    {
        _world.GetBtWorld().removeRigidBody(_btBody);
        Tools::Delete(_btBody);
        Tools::Delete(_motionState);
    }


}}}
