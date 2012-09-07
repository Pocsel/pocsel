#ifndef __COMMON_PHYSICS_BODY_HPP__
#define __COMMON_PHYSICS_BODY_HPP__

#include "common/physics/Node.hpp"

class btRigidBody;
class btTypedConstraint;
struct btDefaultMotionState;

namespace Common { namespace Physics {

    class BodyType;
    class BodyCluster;

    class Body :
        private boost::noncopyable
    {
    public:
        struct BodyNode
        {
            BodyNode() : body(0), motionState(0), constraint(0), dirty(false) {}
            Common::Physics::Node node;
            btRigidBody* body;
            btDefaultMotionState* motionState;
            btTypedConstraint* constraint;
            bool dirty;
        };
    protected:
        BodyCluster& _parent;
        BodyType const& _type;
        std::vector<BodyNode> _nodes;

    public:
        Body(BodyCluster& parent, BodyType const& bodyType);
        ~Body();
        BodyType const& GetType() const { return this->_type; }
        std::vector<BodyNode> const& GetNodes() const { return this->_nodes; }

        btRigidBody& GetRootBtBody();
        btRigidBody const& GetRootBtBody() const;

        void UpdatePosition();

        void Dump() const;

    private:
        void _BuildBodyNode(Uint32 nodeId);
        void _CleanBodyNode(Uint32 nodeId);
        void _UpdateNodePosition(Uint32 nodeId);
    };

}}

#endif
