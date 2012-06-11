#ifndef __TOOLS_PHYSICS_BODY_HPP__
#define __TOOLS_PHYSICS_BODY_HPP__

#include "tools/physics/Node.hpp"

namespace Tools { namespace Physics {

    class BodyType;

    class Body
    {
    public:
        struct BodyNode
        {
            BodyNode() : dirty(false) {}
            Tools::Physics::Node node;
            bool dirty;
        };
    protected:
        BodyType const& _type;
        std::vector<BodyNode> _nodes;

    public:
        Body(BodyType const& bodyType);
        BodyType const& GetType() const { return this->_type; }
        std::vector<BodyNode> const& GetNodes() const { return this->_nodes; }

    };

}}

#endif
