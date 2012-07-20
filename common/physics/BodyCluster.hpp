#ifndef __COMMON_PHYSICS_BODYCLUSTER_HPP__
#define __COMMON_PHYSICS_BODYCLUSTER_HPP__

#include "common/Position.hpp"

class btMotionState;
class btRigidBody;
class btTypedConstraint;

namespace Common { namespace Physics {

    class World;
    class Body;
    struct Node;

    class BodyCluster
    {
    private:
        Common::Physics::World& _world;
        btMotionState* _motionState;
        btRigidBody* _body;
        std::map<Body*, btTypedConstraint*> _constraints;

    public:
        BodyCluster(World& world, Node const& pos);
        ~BodyCluster();

        btRigidBody& GetBody() { return *this->_body; }
        btRigidBody const& GetBody() const { return *this->_body; }
        World& GetWorld() { return this->_world; }

        void AddConstraint(Body* body);
        void RemoveConstraint(Body* body);

        void Dump() const;
    };

}}

#endif
