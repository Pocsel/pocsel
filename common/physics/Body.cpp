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
        for (auto rootsIt = this->_type.GetRoots().begin(),
                rootsIte = this->_type.GetRoots().end();
                rootsIt != rootsIte; ++rootsIt)
        {
            this->_BuildBodyNode(*rootsIt);
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

        btGeneric6DofConstraint* newConstraint = new btGeneric6DofConstraint(*parent, *node.body, thisTr, btTransform::getIdentity(), false);

        for (int i = 0; i < 6; ++i)
        {
            //newConstraint->setParam(BT_CONSTRAINT_STOP_ERP, 0.2, i);
            //newConstraint->setParam(BT_CONSTRAINT_STOP_CFM, 0.04, i);
            //newConstraint->setParam(BT_CONSTRAINT_CFM, 0.1, i);
        }

        //if (false && parent == &this->_parent.GetBody())
        {
            btVector3 anglLimit(0, 0, 0);//-0.01, -0.01, -0.01);//-SIMD_PI * 9 / 10, -SIMD_PI * 9 / 10, -SIMD_PI * 9 / 10);
            newConstraint->setAngularLowerLimit(anglLimit);
        }
        {
            btVector3 anglLimit(0, 0, 0);//0.01, 0.01, 0.01);//SIMD_PI * 9 / 10, SIMD_PI * 9 / 10, SIMD_PI * 9 / 10);
            newConstraint->setAngularUpperLimit(anglLimit);
        }


        node.constraint = newConstraint;

        this->_parent.GetWorld().GetBtWorld().addRigidBody(node.body);
        this->_parent.GetWorld().GetBtWorld().addConstraint(node.constraint, true);

        for (auto childIt = this->_type.GetShapes()[nodeId].children.begin(),
                childIte = this->_type.GetShapes()[nodeId].children.end();
                childIt != childIte; ++childIt)
            this->_BuildBodyNode(*childIt);

        void* userData = this->_parent.GetUserData();
        if (userData)
            node.body->setUserPointer(userData);
    }

    Body::~Body()
    {
        this->_parent.RemoveConstraint(this);
        for (auto rootsIt = this->_type.GetRoots().begin(),
                rootsIte = this->_type.GetRoots().end();
                rootsIt != rootsIte; ++rootsIt)
        {
            this->_CleanBodyNode(*rootsIt);
        }
        //this->_parent.GetWorld().GetBtWorld().removeRigidBody(this->_rootBody);
        //Tools::Delete(this->_rootBody);
        //Tools::Delete(this->_rootMotionState);
    }

    void Body::_CleanBodyNode(Uint32 nodeId)
    {
        for (auto childIt = this->_type.GetShapes()[nodeId].children.begin(),
                childIte = this->_type.GetShapes()[nodeId].children.end();
                childIt != childIte; ++childIt)
            this->_CleanBodyNode(*childIt);

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

    void Body::_RemoveFromWorld()
    {
        for (auto rootsIt = this->_type.GetRoots().begin(),
                rootsIte = this->_type.GetRoots().end();
                rootsIt != rootsIte; ++rootsIt)
        {
            this->_RemoveNodeFromWorld(*rootsIt);
        }
    }

    void Body::_RemoveNodeFromWorld(Uint32 nodeId)
    {
        for (auto childIt = this->_type.GetShapes()[nodeId].children.begin(),
                childIte = this->_type.GetShapes()[nodeId].children.end();
                childIt != childIte; ++childIt)
            this->_RemoveNodeFromWorld(*childIt);

        BodyNode& node = this->_nodes[nodeId];
        this->_parent.GetWorld().GetBtWorld().removeConstraint(node.constraint);
        this->_parent.GetWorld().GetBtWorld().removeRigidBody(node.body);
    }

    void Body::_UpdatePosition()
    {
        for (auto rootsIt = this->_type.GetRoots().begin(),
                rootsIte = this->_type.GetRoots().end();
                rootsIt != rootsIte; ++rootsIt)
        {
            this->_UpdateNodePosition(*rootsIt);
        }
    }

    void Body::_UpdateNodePosition(Uint32 nodeId)
    {
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

        node.body->setLinearVelocity(parent->getVelocityInLocalPoint(
                    tr.getOrigin()
                    -
                    parentTr.getOrigin()
                    ));
        node.body->setAngularVelocity(parent->getAngularVelocity());

        node.body->clearForces();
        node.body->setCenterOfMassTransform(tr);
        node.body->getMotionState()->setWorldTransform(tr);



        //btVector3 vel = parent->getLinearVelocity();
        //node.body->setLinearVelocity(vel);

        //vel = parent->getAngularVelocity();
        //btQuaternion velQ(
        //node.body->setAngularVelocity(thisTr.inverse()(vel));



        for (auto childIt = this->_type.GetShapes()[nodeId].children.begin(),
                childIte = this->_type.GetShapes()[nodeId].children.end();
                childIt != childIte; ++childIt)
            this->_UpdateNodePosition(*childIt);
    }

    void Body::_PutBackInWorld()
    {
        for (auto rootsIt = this->_type.GetRoots().begin(),
                rootsIte = this->_type.GetRoots().end();
                rootsIt != rootsIte; ++rootsIt)
        {
            this->_PutNodeBackInWorld(*rootsIt);
        }
    }

    void Body::_PutNodeBackInWorld(Uint32 nodeId)
    {
        BodyNode& node = this->_nodes[nodeId];
        this->_parent.GetWorld().GetBtWorld().addRigidBody(node.body);
        this->_parent.GetWorld().GetBtWorld().addConstraint(node.constraint, true);
        for (auto childIt = this->_type.GetShapes()[nodeId].children.begin(),
                childIte = this->_type.GetShapes()[nodeId].children.end();
                childIt != childIte; ++childIt)
            this->_PutNodeBackInWorld(*childIt);
    }

    void Body::_ApplyAccel(btVector3 const& accel)
    {
        for (Uint32 rootId: this->_type.GetRoots())
            this->_ApplyAccelOnNode(accel, rootId);
    }

    void Body::_ApplyAccelOnNode(btVector3 const& accel, Uint32 nodeId)
    {
        BodyNode& node = this->_nodes[nodeId];
        //node.body->applyCentralImpulse(accel);
        node.body->setGravity(this->_parent.GetWorld().GetGravity() + accel);
        for (Uint32 childId: this->_type.GetShapes()[nodeId].children)
            this->_ApplyAccelOnNode(accel, childId);
    }

    void Body::Dump() const
    {
        std::cout << "      Body::Dump()\n";

        //{
        //    btTransform tr;
        //    this->_rootMotionState->getWorldTransform(tr);
        //    std::cout << "         " <<
        //        tr.getOrigin().x() << ", " <<
        //        tr.getOrigin().y() << ", " <<
        //        tr.getOrigin().z() << "\n";
        //}

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
