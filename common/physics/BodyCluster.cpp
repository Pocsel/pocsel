#include "common/physics/BodyCluster.hpp"
#include "common/physics/Body.hpp"
#include "common/physics/BodyType.hpp"
#include "common/physics/Utils.hpp"
#include "common/physics/World.hpp"

namespace Common { namespace Physics {

    BodyCluster::BodyCluster(World& world, Node const& pos) :
        _world(world),
        _motionState(0),
        _body(0),
        _userData(0),
        _acceleration(0, 0, 0),
        _accelerationIsLocal(false)
    {
        btScalar mass(0.001);
        btVector3 localInertia(1, 1, 1);

        static std::unique_ptr<btCollisionShape> emptyShape(new btEmptyShape());

        btTransform startTransform;
        startTransform.setIdentity();
        startTransform.setOrigin(btVector3(pos.position.x, pos.position.y, pos.position.z));
        startTransform.setRotation(btQuaternion(pos.orientation.x,
                    pos.orientation.y,
                    pos.orientation.z,
                    pos.orientation.w));

        this->_motionState = new btDefaultMotionState(startTransform);
        btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, _motionState, emptyShape.get(), localInertia);
        this->_body = new btRigidBody(rbInfo);
        this->_body->setActivationState(DISABLE_DEACTIVATION);

        this->_body->setUserPointer(this);

        this->_body->setLinearFactor(btVector3(1,1,1));
        this->_body->setAngularFactor(btVector3(1,1,1));

        this->_world.AddBodyCluster(this);
        this->_world.GetBtWorld().addRigidBody(_body);

        this->_body->setGravity(btVector3(0, 0, 0));
    }

    BodyCluster::~BodyCluster()
    {
        assert(this->_constraints.empty() && "il faut vider les constraints avant de delete le cluster");

        this->_world.RemoveBodyCluster(this);
        this->_world.GetBtWorld().removeRigidBody(this->_body);

        Tools::Delete(_body);
        Tools::Delete(_motionState);
    }

    void BodyCluster::PreTick()
    {
        if (this->_acceleration == btVector3(0, 0, 0))
            return;

        // limitation de la vitesse
        btVector3 velocity = this->_body->getLinearVelocity();
        btQuaternion directionQ = this->_body->getCenterOfMassTransform().getRotation();

        btVector3 accel = this->_acceleration;
        if (this->_accelerationIsLocal)
            accel = quatRotate(directionQ, accel);

        btVector3 targetDirection = accel;
        targetDirection.normalize();

        btScalar speed = velocity.dot(targetDirection);

        if (speed >= this->_maxSpeed)
            return;

        if (this->_body->getInvMass() != 0)
            this->_body->applyCentralForce(accel * (1.0 / this->_body->getInvMass()));
        else
            this->_body->applyCentralForce(accel);

        for (auto body: _constraints)
            body->_ApplyAccel(accel);
    }

    void BodyCluster::PostTick()
    {
        for (auto body: _constraints)
            body->_PostTick();
    }

    void BodyCluster::PreBtTick(btScalar timeStep)
    {
        for (auto body: _constraints)
            body->_PreBtTick(timeStep);
    }

    void BodyCluster::AddConstraint(Body* body)
    {
        this->_constraints.push_back(body);
    }

    void BodyCluster::RemoveConstraint(Body* body)
    {
        for (auto it = this->_constraints.begin(), ite = this->_constraints.end(); it != ite; ++it)
        {
            if (*it == body)
            {
                this->_constraints.erase(it);
                return;
            }
        }
        assert(true && "this body has not been found");
    }

    void BodyCluster::SetPhysics(std::vector<Node> const& physics)
    {
        for (auto& Body: this->_constraints)
        {
            Body->_RemoveFromWorld();
        }

        btRigidBody& btBody = *this->_body;

        this->_world.GetBtWorld().removeRigidBody(&btBody);

        Utils::PhysicsNodeToBtBody(physics[0], btBody);

        if (physics[0].accelerationIsLocal)
            this->SetLocalAccel(
                    btVector3(physics[0].acceleration.x, physics[0].acceleration.y, physics[0].acceleration.z),
                    physics[0].maxSpeed);
        else
            this->SetAccel(
                    btVector3(physics[0].acceleration.x, physics[0].acceleration.y, physics[0].acceleration.z),
                    physics[0].maxSpeed);

        auto physicsIt = physics.begin();
        auto physicsEnd = physics.end();

        for (auto body: this->_constraints)
        {
            if (physicsIt == physicsEnd)
            {
                std::cout << "BLIP BLOP FAIL0\n";
                break;
            }
            for (auto& bodyNode: body->GetNodes())
            {
                ++physicsIt;
                if (physicsIt == physicsEnd)
                {
                    std::cout << "BLIP BLOP FAIL1\n";
                    break;
                }
                Utils::PhysicsNodeToBtBody(*physicsIt, *bodyNode.body);

                const_cast<btVector3&>(bodyNode.acceleration) = btVector3(physicsIt->acceleration.x, physicsIt->acceleration.y, physicsIt->acceleration.z);
                const_cast<double&>(bodyNode.maxSpeed) = physicsIt->maxSpeed;
                const_cast<bool&>(bodyNode.accelerationIsLocal) = physicsIt->accelerationIsLocal;

                const_cast<btVector3&>(bodyNode.interPositionTarget) =
                    btVector3(
                            physicsIt->interPositionTarget.x,
                            physicsIt->interPositionTarget.y,
                            physicsIt->interPositionTarget.z);
                const_cast<double&>(bodyNode.interPositionTargetSpeed) = physicsIt->interPositionTargetSpeed;
                const_cast<btVector3&>(bodyNode.interAngleTarget) =
                    btVector3(
                            physicsIt->interAngleTarget.x,
                            physicsIt->interAngleTarget.y,
                            physicsIt->interAngleTarget.z);
                const_cast<double&>(bodyNode.interAngleTargetSpeed) = physicsIt->interAngleTargetSpeed;

                const_cast<btVector3&>(bodyNode.interPositionCurrent) =
                    btVector3(
                            physicsIt->interPosition.x,
                            physicsIt->interPosition.y,
                            physicsIt->interPosition.z);
                const_cast<btVector3&>(bodyNode.interAngleCurrent) =
                    btVector3(
                            physicsIt->interAngle.x,
                            physicsIt->interAngle.y,
                            physicsIt->interAngle.z);
                const_cast<btVector3&>(bodyNode.interAngleLimits) =
                    btVector3(
                            physicsIt->interAngleLimits.x,
                            physicsIt->interAngleLimits.y,
                            physicsIt->interAngleLimits.z);

                //bodyNode.constraint->setLinearLowerLimit(
                //    btVector3(
                //            physicsIt->interPosition.x,
                //            physicsIt->interPosition.y,
                //            physicsIt->interPosition.z));
                //bodyNode.constraint->setLinearUpperLimit(
                //    btVector3(
                //            physicsIt->interPosition.x,
                //            physicsIt->interPosition.y,
                //            physicsIt->interPosition.z));
                //bodyNode.constraint->setAngularLowerLimit(
                //    btVector3(
                //            physicsIt->interAngle.x,
                //            physicsIt->interAngle.y,
                //            physicsIt->interAngle.z));
                //bodyNode.constraint->setAngularUpperLimit(
                //    btVector3(
                //            physicsIt->interAngle.x,
                //            physicsIt->interAngle.y,
                //            physicsIt->interAngle.z));
            }
        }

        this->_world.GetBtWorld().addRigidBody(&btBody);

        for (auto body: this->_constraints)
        {
            body->_PutBackInWorld();
        }
    }

    void BodyCluster::SetAccel(btVector3 const& accel, btScalar maxSpeed)
    {
        this->_acceleration = accel;
        this->_maxSpeed = maxSpeed;
        this->_accelerationIsLocal = false;
    }

    void BodyCluster::SetLocalAccel(btVector3 const& accel, btScalar maxSpeed)
    {
        this->_acceleration = accel;
        this->_maxSpeed = maxSpeed;
        this->_accelerationIsLocal = true;
    }

    std::vector<Common::Physics::Node> BodyCluster::GetClusterPhysics() const
    {
        std::vector<Common::Physics::Node> physics;

        // noeud maitre
        {
            physics.emplace_back(Utils::BtBodyToPhysicsNode(*this->_body));
            Common::Physics::Node& node = physics.back();
            btVector3 const& accel = this->_acceleration;
            node.acceleration = glm::dvec3(accel.x(), accel.y(), accel.z());
            if (accel != btVector3(0, 0, 0))
            {
                node.accelerationIsLocal = this->_accelerationIsLocal;
                node.maxSpeed = this->_maxSpeed;
            }
        }

        for (Body* body: this->_constraints)
        {
            for (auto& bodyNode: body->GetNodes())
            {
                physics.emplace_back(Utils::BtBodyToPhysicsNode(*bodyNode.body));

                auto& node = physics.back();
                btVector3 const& accel = bodyNode.acceleration;
                node.acceleration = glm::dvec3(accel.x(), accel.y(), accel.z());
                if (accel != btVector3(0, 0, 0))
                {
                    node.accelerationIsLocal = bodyNode.accelerationIsLocal;
                    node.maxSpeed = bodyNode.maxSpeed;
                }

                // inter node shit
                {
                    btVector3 currentPos = bodyNode.interPositionCurrent;
                    //bodyNode.constraint->getLinearLowerLimit(currentPos);
                    node.interPosition =
                        glm::dvec3(
                                currentPos.x(),
                                currentPos.y(),
                                currentPos.z()
                                );
                    currentPos = bodyNode.interAngleCurrent;
                    //bodyNode.constraint->getAngularLowerLimit(currentPos);
                    node.interAngle =
                        glm::dvec3(
                                currentPos.x(),
                                currentPos.y(),
                                currentPos.z()
                                );

                    node.interPositionTarget =
                        glm::dvec3(
                                bodyNode.interPositionTarget.x(),
                                bodyNode.interPositionTarget.y(),
                                bodyNode.interPositionTarget.z()
                                );
                    node.interPositionTargetSpeed = bodyNode.interPositionTargetSpeed;
                    node.interAngleTarget = 
                        glm::dvec3(
                                bodyNode.interAngleTarget.x(),
                                bodyNode.interAngleTarget.y(),
                                bodyNode.interAngleTarget.z()
                                );
                    node.interAngleTargetSpeed = bodyNode.interAngleTargetSpeed;
                    node.interAngleLimits = 
                        glm::dvec3(
                                bodyNode.interAngleLimits.x(),
                                bodyNode.interAngleLimits.y(),
                                bodyNode.interAngleLimits.z()
                                );
                }
            }
        }

        return physics;
    }

    void BodyCluster::SetUserData(void* userData)
    {
        this->_userData = userData;
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

        for (auto& body: this->_constraints)
            body->Dump();
    }

}}
