#include "common/physics/BodyCluster.hpp"
#include "common/physics/Body.hpp"
#include "common/physics/World.hpp"

#include "bullet/bullet-all.hpp"

namespace Common { namespace Physics {

    BodyCluster::BodyCluster(Common::Physics::World& world, Common::Position const& pos) :
        _world(world),
        _motionState(0),
        _body(0)
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
        startTransform.setOrigin(btVector3(pos.x, pos.y, pos.z));

        _motionState = new btDefaultMotionState(startTransform);
        btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, _motionState, emptyShape, localInertia);
        _body = new btRigidBody(rbInfo);

        _body->setUserPointer(this);

        this->_world.GetBtWorld().addRigidBody(_body);
    }

    BodyCluster::~BodyCluster()
    {
        assert(this->_constraints.empty() && "il faut vider les constraints avant de delete le cluster");

        this->_world.GetBtWorld().removeRigidBody(this->_body);

        Tools::Delete(_body);
        Tools::Delete(_motionState);
    }

    void BodyCluster::AddConstraint(Body* body)
    {
        assert(this->_constraints.count(body) == 0);

        btTypedConstraint*& constraint = this->_constraints[body];
        btTransform identity;
        identity.setIdentity();
        constraint = new btGeneric6DofConstraint(*this->_body, body->GetRootBtBody(), identity, identity, false);

        this->_world.GetBtWorld().addConstraint(constraint, true);
    }

    void BodyCluster::RemoveConstraint(Body* body)
    {
        assert(this->_constraints.count(body) == 1);

        btTypedConstraint* constraint = this->_constraints[body];
        this->_constraints.erase(body);

        this->_world.GetBtWorld().removeConstraint(constraint);
        Tools::Delete(constraint);
    }

}}
