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
        std::vector<Body*> _constraints;

        void* _userData;

        float _curMass;

    public:
        BodyCluster(World& world, Node const& pos);
        ~BodyCluster();

        void Tick();

        btRigidBody& GetBtBody() { return *this->_body; }
        btRigidBody const& GetBtBody() const { return *this->_body; }
        World& GetWorld() { return this->_world; }

        void AddConstraint(Body* body);
        void RemoveConstraint(Body* body);

        void SetPhysics(Node const& physics);

        void SetUserData(void* userData);
        void* GetUserData() { return this->_userData; }

        void Dump() const;
    };

}}

#endif
