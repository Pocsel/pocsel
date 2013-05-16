#include "common/physics/Body.hpp"
#include "common/physics/BodyType.hpp"
#include "common/physics/World.hpp"
#include "common/physics/BodyCluster.hpp"

#include "bullet/bullet-all.hpp"


namespace Common { namespace Physics {

    Body::Body(BodyCluster& parent, BodyType const& bodyType) :
        _parent(parent),
        _type(bodyType),
        _nodes(bodyType.GetShapes().size())
    {
        for (auto root: this->_type.GetRoots())
        {
            this->_BuildBodyNode(root);
        }
        this->_parent.AddConstraint(this);

       // this->Dump();
    }

    void Body::_BuildBodyNode(Uint32 nodeId)
    {
        // step 1 - créer le body (avec la constraint sur le parent)
        // step 2 - créer les fils (avec eux-meme qui créent leur constraint)
        BodyNode& node = this->_nodes[nodeId];
        BodyType::ShapeNode const& shape = this->_type.GetShapes()[nodeId];
        btRigidBody* parent = shape.parent == -1 ? &this->_parent.GetBtBody() : this->_nodes[shape.parent].body;

        btTransform parentTr;
        parent->getMotionState()->getWorldTransform(parentTr);

        btTransform thisTr;
        thisTr.setIdentity();
        thisTr.setOrigin(btVector3(
                    shape.position.position.x,
                    shape.position.position.y,
                    shape.position.position.z));
        thisTr.setRotation(btQuaternion(
                    shape.position.orientation.x,
                    shape.position.orientation.y,
                    shape.position.orientation.z,
                    shape.position.orientation.w));

        btTransform tr;
        tr.mult(parentTr, thisTr);
        node.motionState = new btDefaultMotionState(tr);

        btScalar mass(shape.mass);
        btVector3 localInertia(0, 0, 0);

        shape.shape->calculateLocalInertia(mass, localInertia);

        btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, node.motionState, shape.shape, localInertia);
        rbInfo.m_friction = shape.friction;
        rbInfo.m_restitution = shape.restitution;
        node.body = new btRigidBody(rbInfo);
        node.body->setActivationState(DISABLE_DEACTIVATION);

        node.body->setLinearFactor(btVector3(1,1,1));
        node.body->setAngularFactor(btVector3(1,1,1));

        node.interBaseTransform = thisTr;

        btGeneric6DofConstraint* newConstraint = new btGeneric6DofConstraint(*parent, *node.body, thisTr, btTransform::getIdentity(), false);

        for (int i = 0; i < 6; ++i)
        {
            //newConstraint->setParam(BT_CONSTRAINT_STOP_ERP, 0.2, i);
            //newConstraint->setParam(BT_CONSTRAINT_STOP_CFM, 0.04, i);
            //newConstraint->setParam(BT_CONSTRAINT_CFM, 0.1, i);
        }

        {
            //btVector3 anglLimit(0, 0, 0);
            newConstraint->setAngularLowerLimit(-node.interAngleLimits);
        }
        {
            //btVector3 anglLimit(0, 0, 0);
            newConstraint->setAngularUpperLimit(node.interAngleLimits);
        }


        node.constraint = newConstraint;

        this->_parent.GetWorld().GetBtWorld().addRigidBody(node.body);
        this->_parent.GetWorld().GetBtWorld().addConstraint(node.constraint);// , true);

        for (auto child: this->_type.GetShapes()[nodeId].children)
            this->_BuildBodyNode(child);

        node.body->setUserPointer(&this->_parent);
    }

    Body::~Body()
    {
        this->_parent.RemoveConstraint(this);
        for (auto root: this->_type.GetRoots())
        {
            this->_CleanBodyNode(root);
        }
    }

    void Body::_CleanBodyNode(Uint32 nodeId)
    {
        for (auto child: this->_type.GetShapes()[nodeId].children)
            this->_CleanBodyNode(child);

        BodyNode& node = this->_nodes[nodeId];
        this->_parent.GetWorld().GetBtWorld().removeConstraint(node.constraint);
        Tools::Delete(node.constraint);
        this->_parent.GetWorld().GetBtWorld().removeRigidBody(node.body);
        Tools::Delete(node.body);
        Tools::Delete(node.motionState);
    }

    btRigidBody& Body::GetRootBtBody()
    {
        return this->_parent.GetBtBody();
    }

    btRigidBody const& Body::GetRootBtBody() const
    {
        return this->_parent.GetBtBody();
    }

    void Body::SetAccel(std::string const& node, glm::dvec3 const& accel, double maxSpeed)
    {
        auto& bodyNode = this->_GetNode(node);

        bodyNode.acceleration = btVector3(accel.x, accel.y, accel.z);
        bodyNode.maxSpeed = maxSpeed;
        bodyNode.accelerationIsLocal = false;
    }

    void Body::SetLocalAccel(std::string const& node, glm::dvec3 const& accel, double maxSpeed)
    {
        auto& bodyNode = this->_GetNode(node);

        bodyNode.acceleration = btVector3(accel.x, accel.y, accel.z);
        bodyNode.maxSpeed = maxSpeed;
        bodyNode.accelerationIsLocal = true;
    }

    void Body::SetInterPositionTarget(std::string const& node, glm::dvec3 const& accel, double maxSpeed)
    {
        auto& bodyNode = this->_GetNode(node);

        bodyNode.interPositionTarget = btVector3(accel.x, accel.y, accel.z);
        bodyNode.interPositionTargetSpeed = maxSpeed;
    }

    void Body::SetInterAngleTarget(std::string const& node, glm::dvec3 const& accel, double maxSpeed)
    {
        auto& bodyNode = this->_GetNode(node);

        bodyNode.interAngleTarget = btVector3(
                btNormalizeAngle(accel.x),
                btNormalizeAngle(accel.y),
                btNormalizeAngle(accel.z)
                );

        bodyNode.interAngleTargetSpeed = maxSpeed;
    }

    void Body::SetInterAngleLimits(std::string const& node, glm::dvec3 const& limits)
    {
        auto& bodyNode = this->_GetNode(node);

        bodyNode.interAngleLimits = btVector3(
                btNormalizeAngle(limits.x),
                btNormalizeAngle(limits.y),
                btNormalizeAngle(limits.z)
                );
    }

    Body::BodyNode& Body::_GetNode(std::string const& nodeName)
    {
        Int32 nodeIdx = this->_type.GetNodeIndex(nodeName);
        if (nodeIdx == -1)
            throw std::runtime_error("Node \"" + nodeName + "\" does not exist");
        return this->_nodes[nodeIdx];
    }

    void Body::_RemoveFromWorld()
    {
        for (auto root: this->_type.GetRoots())
        {
            this->_RemoveNodeFromWorld(root);
        }
    }

    void Body::_RemoveNodeFromWorld(Uint32 nodeId)
    {
        for (auto child: this->_type.GetShapes()[nodeId].children)
            this->_RemoveNodeFromWorld(child);

        BodyNode& node = this->_nodes[nodeId];
        this->_parent.GetWorld().GetBtWorld().removeConstraint(node.constraint);
        this->_parent.GetWorld().GetBtWorld().removeRigidBody(node.body);
    }

    void Body::_PutBackInWorld()
    {
        for (auto root: this->_type.GetRoots())
        {
            this->_PutNodeBackInWorld(root);
        }
    }

    void Body::_PutNodeBackInWorld(Uint32 nodeId)
    {
        BodyNode& node = this->_nodes[nodeId];
        this->_parent.GetWorld().GetBtWorld().addRigidBody(node.body);
        this->_parent.GetWorld().GetBtWorld().addConstraint(node.constraint);//, true);
        for (auto child: this->_type.GetShapes()[nodeId].children)
            this->_PutNodeBackInWorld(child);
    }

    void Body::_ApplyAccel(btVector3 const& accel)
    {
        for (Uint32 rootId: this->_type.GetRoots())
            this->_ApplyAccelOnNode(accel, rootId);
    }

    void Body::_ApplyAccelOnNode(btVector3 const& accel, Uint32 nodeId)
    {
        BodyNode& node = this->_nodes[nodeId];

        btVector3 realAccel = accel;

        if (node.acceleration != btVector3(0, 0, 0))
        {
            btVector3 velocity = node.body->getLinearVelocity();

            btQuaternion directionQ = node.body->getCenterOfMassTransform().getRotation();
            btVector3 nodeAccel = node.acceleration;
            if (node.accelerationIsLocal)
                nodeAccel = quatRotate(directionQ, nodeAccel);
            btVector3 targetDirection = nodeAccel;
            targetDirection.normalize();
            btScalar speed = velocity.dot(targetDirection);

            if (speed < node.maxSpeed)
            {
                realAccel += nodeAccel;
            }
        }

        if (node.body->getInvMass() != 0)
            node.body->applyCentralForce(accel * (1.0 / node.body->getInvMass()));
        else
            node.body->applyCentralForce(accel);

        for (Uint32 childId: this->_type.GetShapes()[nodeId].children)
            this->_ApplyAccelOnNode(accel, childId);
    }

    void Body::_PostTick()
    {
    }

    void Body::_PreBtTick(btScalar timeStep)
    {
        bool change;
        for (BodyNode& node: this->_nodes)
        {
            change = false;

            if (node.interPositionTarget != node.interPositionCurrent)
            {
                change = true;

                if (node.interPositionCurrent.distance(node.interPositionTarget) <= node.interPositionTargetSpeed * timeStep)
                {
                    node.interPositionCurrent = node.interPositionTarget;
                }
                else
                {
                    node.interPositionCurrent = node.interPositionCurrent + (node.interPositionTarget - node.interPositionCurrent).normalized() * node.interPositionTargetSpeed * timeStep;
                }
            }

            if (node.interAngleTarget != node.interAngleCurrent)
            {
                change = true;

                btVector3 difference = node.interAngleTarget - node.interAngleCurrent;
                for (unsigned int i = 0; i < 3; ++i)
                {
                    if (difference[i] > SIMD_PI)
                        difference[i] -= SIMD_2_PI;
                    else if (difference[i] < -SIMD_PI)
                        difference[i] += SIMD_2_PI;
                }

                if (difference.length() <= node.interAngleTargetSpeed * timeStep)
                {
                    node.interAngleCurrent = node.interAngleTarget;
                }
                else
                {
                    node.interAngleCurrent = node.interAngleCurrent + difference.normalized() * node.interAngleTargetSpeed * timeStep;
                    for (unsigned int i = 0; i < 3; ++i)
                        btNormalizeAngle(node.interAngleCurrent[i]);
                }
            }

            if (node.interAngleLimits != node.interAngleLimitsCurrent)
            {
                node.constraint->setAngularLowerLimit(-node.interAngleLimits);
                node.constraint->setAngularUpperLimit(node.interAngleLimits);
                node.interAngleLimitsCurrent = node.interAngleLimits;
            }

            if (change)
            {
                btTransform tr;
                tr.setIdentity();
                tr.setOrigin(node.interPositionCurrent);
                tr.setRotation(btQuaternion(node.interAngleCurrent.x(), node.interAngleCurrent.y(), node.interAngleCurrent.z()));

                node.constraint->setFrames(node.interBaseTransform * tr, btTransform::getIdentity());
            }

        }
        //btVector3 curTarget;
        //btVector3 newTarget;
        //for (BodyNode& node: this->_nodes)
        //{
        //    node.constraint->getLinearLowerLimit(curTarget);
        //    if (node.interPositionTarget != curTarget)
        //    {
        //        if (curTarget.distance(node.interPositionTarget) <= node.interPositionTargetSpeed * timeStep)
        //        {
        //            node.constraint->setLinearLowerLimit(node.interPositionTarget);
        //            node.constraint->setLinearUpperLimit(node.interPositionTarget);
        //        }
        //        else
        //        {
        //            newTarget = curTarget + (node.interPositionTarget - curTarget).normalized() * node.interPositionTargetSpeed * timeStep;
        //            node.constraint->setLinearLowerLimit(newTarget);
        //            node.constraint->setLinearUpperLimit(newTarget);
        //        }
        //    }

        //    node.constraint->getAngularLowerLimit(curTarget);
        //    if (node.interAngleTarget != curTarget)
        //    {
        //        //std::cout << "XXX\n";
        //        //std::cout << "  target " << node.interAngleTarget.x() << ", cur " << curTarget.x() << "\n";
        //        //std::cout << "  target " << node.interAngleTarget.y() << ", cur " << curTarget.y() << "\n";
        //        //std::cout << "  target " << node.interAngleTarget.z() << ", cur " << curTarget.z() << "\n";
        //        // TODO regler le bug qui fait que y'a grave de la merde
        //        if (curTarget.distance(node.interAngleTarget) <= node.interAngleTargetSpeed * timeStep)
        //        {
        //            node.constraint->setAngularLowerLimit(node.interAngleTarget);
        //            node.constraint->setAngularUpperLimit(node.interAngleTarget);
        //        }
        //        else
        //        {
        //            newTarget = curTarget + (node.interAngleTarget - curTarget).normalized() * node.interAngleTargetSpeed * timeStep;
        //            node.constraint->setAngularLowerLimit(newTarget);
        //            node.constraint->setAngularUpperLimit(newTarget);
        //        }
        //    }
        //}
    }

    void Body::Dump() const
    {
        std::cout << "      Body::Dump()\n";

        for (auto it = this->_nodes.begin(), ite = this->_nodes.end(); it != ite; ++it)
        {
            btTransform tr;
            it->motionState->getWorldTransform(tr);
            std::cout << "             " <<
                tr.getOrigin().x() << ", " <<
                tr.getOrigin().y() << ", " <<
                tr.getOrigin().z() << "\n";
        }
    }

}}
