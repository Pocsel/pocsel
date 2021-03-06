#ifndef __COMMON_PHYSICS_BODY_HPP__
#define __COMMON_PHYSICS_BODY_HPP__

#include "common/physics/Node.hpp"
#include "bullet/bullet-all.hpp"

namespace Common { namespace Physics {

    class BodyType;
    class BodyCluster;

    class Body :
        private boost::noncopyable
    {
    public:
        struct BodyNode
        {
            BodyNode() :
                body(0),
                motionState(0),
                constraint(0),
                dirty(false),
                acceleration(0, 0, 0),
                accelerationIsLocal(false),
                interPositionTarget(0, 0, 0),
                interAngleTarget(0, 0, 0),
                interPositionCurrent(0, 0, 0),
                interAngleCurrent(0, 0, 0),
                interAngleLimits(0, 0, 0),
                interAngleLimitsCurrent(0, 0, 0)
            {}

            Common::Physics::Node node;
            btRigidBody* body;
            btDefaultMotionState* motionState;
            //btTypedConstraint* constraint;
            btGeneric6DofConstraint* constraint;
            bool dirty;

            btVector3 acceleration;
            btScalar maxSpeed;
            bool accelerationIsLocal;

            //trucs inter-noeuds
            btVector3 interPositionTarget;
            btScalar interPositionTargetSpeed;
            btVector3 interAngleTarget;
            btScalar interAngleTargetSpeed;
            btVector3 interPositionCurrent;
            btVector3 interAngleCurrent;
            btVector3 interAngleLimits;
            btVector3 interAngleLimitsCurrent;

            btTransform interBaseTransform;
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

        void SetAccel(std::string const& node, glm::dvec3 const& accel, double maxSpeed);
        void SetLocalAccel(std::string const& node, glm::dvec3 const& accel, double maxSpeed);
        void SetInterPositionTarget(std::string const& node, glm::dvec3 const& accel, double maxSpeed);
        void SetInterAngleTarget(std::string const& node, glm::dvec3 const& accel, double maxSpeed);
        void SetInterAngleLimits(std::string const& node, glm::dvec3 const& limits);

        void Dump() const;

    private:
        BodyNode& _GetNode(std::string const& nodeName);
        void _BuildBodyNode(Uint32 nodeId);
        void _CleanBodyNode(Uint32 nodeId);

        // les trucs de BodyCluster
    private:
        void _ApplyAccel(btVector3 const& accel);
        void _ApplyAccelOnNode(btVector3 const& accel, Uint32 nodeId);

        virtual void _PostTick();
        void _PreBtTick(btScalar timeStep);

        void _RemoveFromWorld();
        void _RemoveNodeFromWorld(Uint32 nodeId);
        void _PutBackInWorld();
        void _PutNodeBackInWorld(Uint32 nodeId);

        friend class BodyCluster;
    };

}}

#endif
