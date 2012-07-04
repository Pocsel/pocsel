#include "common/physics/Body.hpp"
#include "common/physics/BodyType.hpp"
#include "common/physics/World.hpp"

#include "bullet/bullet-all.hpp"


namespace Common { namespace Physics {

    Body::Body(World& world, Common::Physics::Node const& position, BodyType const& bodyType) :
        _world(world),
        _type(bodyType),
        _nodes(bodyType.GetShapes().size()),
        _rootBody(0),
        _rootMotionState(0)
    {
        static btCollisionShape* emptyShape = new btEmptyShape();

        btTransform tr;
        tr.setIdentity();
        tr.setOrigin(btVector3(
                    position.position.x,
                    position.position.y,
                    position.position.z));
        tr.setRotation(btQuaternion(
                    position.orientation.x,
                    position.orientation.y,
                    position.orientation.z,
                    position.orientation.w));

        this->_rootMotionState = new btDefaultMotionState(tr);
        btRigidBody::btRigidBodyConstructionInfo rootInfo(0.00001, this->_rootMotionState, emptyShape, btVector3());
        this->_rootBody = new btRigidBody(rootInfo);
        for (auto rootsIt = this->_type.GetRoots().begin(),
                rootsIte = this->_type.GetRoots().end();
                rootsIt != rootsIte; ++rootsIt)
        {
            this->_BuildBodyNode(*rootsIt);
        }
    }

    void Body::_BuildBodyNode(Uint32 nodeId)
    {
        // step 1 - créer le body (avec la constraint sur le parent)
        // step 2 - créer les fils (avec eux-meme qui créent leur constraint)
        BodyNode& node = this->_nodes[nodeId];
        BodyType::ShapeNode const& shape = this->_type.GetShapes()[nodeId];
        btRigidBody* parent = shape.parent == -1 ? this->_rootBody : this->_nodes[shape.parent].body;

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

        btRigidBody::btRigidBodyConstructionInfo rbInfo(60, node.motionState, shape.shape, btVector3());
        node.body = new btRigidBody(rbInfo);

        node.constraint = new btGeneric6DofConstraint(*parent, *node.body, thisTr, thisTr, false);

        for (auto childIt = this->_type.GetShapes()[nodeId].children.begin(),
                childIte = this->_type.GetShapes()[nodeId].children.end();
                childIt != childIte; ++childIt)
            this->_BuildBodyNode(*childIt);
    }

    Body::~Body()
    {
        for (auto rootsIt = this->_type.GetRoots().begin(),
                rootsIte = this->_type.GetRoots().end();
                rootsIt != rootsIte; ++rootsIt)
        {
            this->_CleanBodyNode(*rootsIt);
        }
        this->_world.GetBtWorld().removeRigidBody(this->_rootBody);
        Tools::Delete(this->_rootBody);
        Tools::Delete(this->_rootMotionState);
    }

    void Body::_CleanBodyNode(Uint32 nodeId)
    {
        for (auto childIt = this->_type.GetShapes()[nodeId].children.begin(),
                childIte = this->_type.GetShapes()[nodeId].children.end();
                childIt != childIte; ++childIt)
            this->_CleanBodyNode(*childIt);

        BodyNode& node = this->_nodes[nodeId];
        this->_world.GetBtWorld().removeConstraint(node.constraint);
        Tools::Delete(node.constraint);
        this->_world.GetBtWorld().removeRigidBody(node.body);
        Tools::Delete(node.body);
    }

}}
