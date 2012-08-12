#include "common/physics/BodyCluster.hpp"
#include "common/physics/Body.hpp"
#include "common/physics/World.hpp"

#include "bullet/bullet-all.hpp"

namespace Common { namespace Physics {

    BodyCluster::BodyCluster(World& world, Node const& pos) :
        _world(world),
        _motionState(0),
        _body(0),
        _userData(0)
    {
        btScalar mass(0.000000001);
        btVector3 localInertia(0.000000001, 0.000000001, 0.000000001);

        static std::unique_ptr<btCollisionShape> emptyShape(new btEmptyShape()); //btSphereShape(1);//btEmptyShape();
        //emptyShape->calculateLocalInertia(mass, localInertia);
        //std::cout << "INERTIAAIAIAIAI " << localInertia.x() << ", " << localInertia.y() << ", " << localInertia.z() << "\n";

        btTransform startTransform;
        startTransform.setIdentity();
        startTransform.setOrigin(btVector3(pos.position.x, pos.position.y, pos.position.z));
        startTransform.setRotation(btQuaternion(pos.orientation.x,
                    pos.orientation.y,
                    pos.orientation.z,
                    pos.orientation.w));

        _motionState = new btDefaultMotionState(startTransform);
        btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, _motionState, emptyShape.get(), localInertia);
        _body = new btRigidBody(rbInfo);
        _body->setActivationState(DISABLE_DEACTIVATION);

        _body->setUserPointer(this);

        _body->setLinearFactor(btVector3(1,1,1));
        _body->setAngularFactor(btVector3(1,1,1));

        this->_world.GetBtWorld().addRigidBody(_body);
        this->_body->setGravity(btVector3(0, 0, 0));
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
    }

    void BodyCluster::RemoveConstraint(Body* body)
    {
        assert(this->_constraints.count(body) == 1);

        btTypedConstraint* constraint = this->_constraints[body];
        this->_constraints.erase(body);
    }

    void BodyCluster::Dump() const
    {
        std::cout << "BodyCluster::Dump()\n";

        btTransform tr;
        this->_motionState->getWorldTransform(tr);
        std::cout << "    " <<
            tr.getOrigin().x() << ", " <<
            tr.getOrigin().y() << ", " <<
            tr.getOrigin().z() << "\n";

        for (auto it = this->_constraints.begin(), ite = this->_constraints.end(); it != ite; ++it)
            it->first->Dump();
    }

    void BodyCluster::SetUserData(void* userData)
    {
        this->_userData = userData;
        this->_body->setUserPointer(userData);
    }

}}
