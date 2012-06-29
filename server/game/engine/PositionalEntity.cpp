#include "server/game/engine/PositionalEntity.hpp"

#include "bullet/bullet-all.hpp"

namespace Server { namespace Game { namespace Engine {

    PositionalEntity::PositionalEntity(Tools::Lua::Interpreter& interpreter, Uint32 id, EntityType const& type, Common::Position const& pos) :
        Entity(interpreter, id, type),
        _btBody(0)
    {
        _physics.position = pos;
    }

    PositionalEntity::~PositionalEntity()
    {
        Tools::Delete(_btBody);
        for (auto it = this->_doodadBodies.begin(), ite = this->_doodadBodies.end(); it != ite; ++it)
        {
            Tools::Delete(it->second.constraint);
        }
    }

    void PositionalEntity::AddConstraint(btRigidBody* doodadBody)
    {
        if (this->_btBody == 0)
        {
            btScalar mass(0.00000001);
            btVector3 localInertia(0, 0, 0);

            static btCollisionShape* emptyShape = 0;
            if (emptyShape == 0)
            {
                emptyShape = new btEmptyShape();
                emptyShape->calculateLocalInertia(mass, localInertia);
            }

            btTransform startTransform;
            startTransform.setIdentity();
            startTransform.setOrigin(btVector3(
                        btScalar(pos.x),
                        btScalar(pos.y),
                        btScalar(pos.z)));

            btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
            btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, emptyShape, localInertia);
            _btBody = new btRigidBody(rbInfo);

            _btBody->setUserPointer(this);
        }
        DoodadBody& doodadBodyConstraint = this->_doodadBodies[doodadBody];
        doodadBodyConstraint.root = doodadBody;
        btTransform identity;
        identity.setIdentity();
        doodadBodyConstraint.constraint = new btGeneric6DofConstraint(*this->_btBody, *doodadBody, identity, identity, false);
    }

    std::unique_ptr<btTypedConstraint> PositionalEntity::PopConstraint(btRigidBody const* doodadBody)
    {
        DoodadBody& doodadBodyConstraint = _doodadBodies[doodadBody];
        btTypedConstraint* constraint = doodadBodyConstraint;
        // TODO j'en suis là
    }

}}}
