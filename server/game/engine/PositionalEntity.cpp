#include "server/game/engine/PositionalEntity.hpp"

#include "bullet/bullet-all.hpp"

namespace Server { namespace Game { namespace Engine {

    PositionalEntity::PositionalEntity(Tools::Lua::Interpreter& interpreter, Uint32 id, EntityType const& type, Common::Position const& pos) :
        Entity(interpreter, id, type),
        _btBody(0)
    {
        _physics.position = pos;

        static btCollisionShape* colShape = new
            // btSphereShape(2) // rayon
            btBoxShape(btVector3(1, 2.5, 1)) // on donne la moitié de la taille
            ;

        /// Create Dynamic Objects

        btScalar mass(100);
        btVector3 localInertia(0, 0, 0);

        colShape->calculateLocalInertia(mass, localInertia);

        btTransform startTransform;
        startTransform.setIdentity();
        startTransform.setOrigin(btVector3(
                    btScalar(pos.x),
                    btScalar(pos.y),
                    btScalar(pos.z)));

        //using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
        btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
        //myMotionState->setWorldTransform(startTransform);
        btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);
        _btBody = new btRigidBody(rbInfo);

        _btBody->setUserPointer(this);
    }

    PositionalEntity::~PositionalEntity()
    {
        Tools::Delete(_btBody);
    }


}}}
