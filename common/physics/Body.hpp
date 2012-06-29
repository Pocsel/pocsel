#ifndef __COMMON_PHYSICS_BODY_HPP__
#define __COMMON_PHYSICS_BODY_HPP__

#include "common/physics/Node.hpp"

class btRigidBody;

namespace Common { namespace Physics {
    class BodyType;
}}

namespace Common { namespace Physics {

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
        /*
        btRigidBody* _btBody;
        */

    public:
        Body(BodyType const& bodyType);//, Common::Physics::Node const& pos);
        ~Body();
        BodyType const& GetType() const { return this->_type; }
        std::vector<BodyNode> const& GetNodes() const { return this->_nodes; }

        BtRigidBody& GetRootBtBody();

    };

}}

#endif
