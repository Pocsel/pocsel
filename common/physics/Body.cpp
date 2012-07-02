#include "common/physics/Body.hpp"
#include "common/physics/BodyType.hpp"
#include "common/physics/World.hpp"

#include "bullet/bullet-all.hpp"


namespace Common { namespace Physics {

    Body::Body(World& world, BodyType const& bodyType) :
        _world(world),
        _type(bodyType),
        _nodes(bodyType.GetShapes().size())
    {
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
    }

    void Body::_CleanBodyNode(Uint32 nodeId)
    {
        for (auto childIt = this->_type.GetShapes()[nodeId].children.begin(),
                childIte = this->_type.GetShapes()[nodeId].children.end();
                childIt != childIte; ++childIt)
            this->_CleanBodyNode(*childId);
        BodyNode& node = this->_nodes[nodeId];
        this->_world.GetBtWorld().RemoveConstraint(node.constraint);
        Tools::Delete(node.constraint);
        this->_world.GetBtWorld().RemoveRigidBody(node.body);
        Tools::Delete(node.body);
    }

}}
