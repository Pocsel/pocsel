#ifndef __COMMON_PHYSICS_BODY_HPP__
#define __COMMON_PHYSICS_BODY_HPP__

#include "common/physics/Node.hpp"

class btRigidBody;
class btTypedConstraint;

namespace Common { namespace Physics {
    class BodyType;
}}

namespace Common { namespace Physics {

    class World;

    class Body
    {
    public:
        struct BodyNode
        {
            BodyNode() : dirty(false) {}
            Common::Physics::Node node;
            btRigidBody* body;
            btTypedConstraint* constraint;
            bool dirty;
        };
    protected:
        World& _world;
        BodyType const& _type;
        std::vector<BodyNode> _nodes;

    public:
        Body(World& world, BodyType const& bodyType);//, Common::Physics::Node const& pos);
        ~Body();
        BodyType const& GetType() const { return this->_type; }
        std::vector<BodyNode> const& GetNodes() const { return this->_nodes; }

        btRigidBody& GetRootBtBody();
    };

}}

#endif
