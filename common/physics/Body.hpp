#ifndef __COMMON_PHYSICS_BODY_HPP__
#define __COMMON_PHYSICS_BODY_HPP__

#include "common/physics/Node.hpp"

namespace Common { namespace Physics {

    class BodyType;

    class Body
    {
    public:
        struct BodyNode
        {
            BodyNode() : dirty(false) {}
            Common::Physics::Node node;
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
