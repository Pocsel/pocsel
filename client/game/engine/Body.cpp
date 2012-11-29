#include "client/game/engine/Body.hpp"
#include "client/game/engine/BodyType.hpp"
#include "common/physics/BodyCluster.hpp"

namespace Client { namespace Game { namespace Engine {

    Body::Body(Common::Physics::BodyCluster& parent, BodyType const& bodyType) :
        Common::Physics::Body(parent, bodyType)
    {
        this->_boundNodes.resize(this->_nodes.size());
    }

    std::shared_ptr<Common::Physics::Node> Body::BindNode(std::string const& nodeName)
    {
        unsigned int idx = 0;
        for (auto& node: this->_type.GetShapes())
        {
            if (node.name == nodeName)
            {
                std::shared_ptr<Common::Physics::Node> boundNode(new Common::Physics::Node());
                this->_boundNodes[idx] = boundNode;
                return boundNode;
            }
            ++idx;
        }
        return std::shared_ptr<Common::Physics::Node>(0);
    }

    void Body::_PostTick()
    {
        for (unsigned int idx = 0; idx < this->_boundNodes.size(); ++idx)
        {
            auto& boundNode = this->_boundNodes[idx];
            if (!boundNode)
                continue;
            BodyNode& node = this->_nodes[idx];
            BodyType::ShapeNode const& shape = this->_type.GetShapes()[idx];
            btRigidBody& parent = shape.parent == -1 ? this->_parent.GetBtBody() : *this->_nodes[shape.parent].body;

            btTransform parentTr;
            parent.getMotionState()->getWorldTransform(parentTr);

            btTransform thisTr;
            node.motionState->getWorldTransform(thisTr);

            btTransform finalTr = node.constraint->getFrameOffsetA().inverseTimes(parentTr.inverseTimes(thisTr));
            btVector3 finalPos = finalTr.getOrigin();
            btQuaternion finalRot = finalTr.getRotation();

            boundNode->position = glm::dvec3(-finalPos.x(), finalPos.y(), -finalPos.z());
            boundNode->orientation = glm::dquat(-finalRot.w(), finalRot.x(), -finalRot.y(), finalRot.z());
        }
    }

}}}
