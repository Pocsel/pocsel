#include "server/game/engine/PositionalEntity.hpp"

#include "bullet/bullet-all.hpp"
#include "common/physics/World.hpp"

#include "common/physics/World.hpp"

namespace Server { namespace Game { namespace Engine {

    PositionalEntity::PositionalEntity(Common::Physics::World& world, Tools::Lua::Interpreter& interpreter, Uint32 id, EntityType const& type, Common::Position const& pos) :
        Entity(interpreter, id, type),
        _world(world),
        _body(0),
        _motionState(0)
    {
        _physics.position = pos;
    }

    PositionalEntity::~PositionalEntity()
    {
        assert(_doodadBodies.empty() && "il faut vider les constraints avant de delete l'entity");

        if (this->_body)
        {
            this->_world.GetBtWorld().removeRigidBody(this->_body);

            Tools::Delete(_body);
            Tools::Delete(_motionState);
        }
    }

    void PositionalEntity::UpdatePhysics()
    {
        btRigidBody const& btBody = *this->_body;
        Common::Physics::Node& physics = this->_physics;

        btTransform wt;
        btBody.getMotionState()->getWorldTransform(wt);

        btVector3 wpos = wt.getOrigin();
        physics.position = Common::Position(wpos.x(), wpos.y(), wpos.z());

        btQuaternion const& wrot = wt.getRotation();
        physics.orientation = glm::quat((float)wrot.w(), (float)wrot.x(), (float)wrot.y(), (float)wrot.z());

        btVector3 const& btVel = btBody.getLinearVelocity();
        physics.velocity = glm::vec3(btVel.x(), btVel.y(), btVel.z());

        btVector3 const& av = btBody.getAngularVelocity();
        physics.angularVelocity = glm::vec3(av.x(), av.y(), av.z());
    }

    void PositionalEntity::AddConstraint(btRigidBody* doodadBody)
    {
        assert(this->_doodadBodies.count(doodadBody) == 0);

        if (this->_body == 0)
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
                        btScalar(_physics.position.x),
                        btScalar(_physics.position.y),
                        btScalar(_physics.position.z)));

            _motionState = new btDefaultMotionState(startTransform);
            btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, _motionState, emptyShape, localInertia);
            _body = new btRigidBody(rbInfo);

            _body->setUserPointer(this);

            this->_world.GetBtWorld().addRigidBody(_body);
        }

        btTypedConstraint*& constraint = this->_doodadBodies[doodadBody];
        btTransform identity;
        identity.setIdentity();
        constraint = new btGeneric6DofConstraint(*this->_body, *doodadBody, identity, identity, false);

        this->_world.GetBtWorld().addConstraint(constraint, true);
    }

    void PositionalEntity::PopConstraint(btRigidBody const* doodadBody)
    {
        assert(this->_doodadBodies.count(doodadBody) == 1);

        btTypedConstraint* constraint = _doodadBodies[doodadBody];
        this->_doodadBodies.erase(doodadBody);

        this->_world.GetBtWorld().removeConstraint(constraint);
        Tools::Delete(constraint);
    }

}}}
