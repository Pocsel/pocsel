#ifndef __COMMON_PHYSICS_BODYCLUSTER_HPP__
#define __COMMON_PHYSICS_BODYCLUSTER_HPP__

#include "bullet/bullet-all.hpp"

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

        btVector3 _acceleration;
        btScalar _maxSpeed;
        bool _accelerationIsLocal;

    public:
        BodyCluster(World& world, Node const& pos);
        ~BodyCluster();

        void PreTick();
        void PostTick();
        void PreBtTick(btScalar timeStep);

        btRigidBody& GetBtBody() { return *this->_body; }
        btRigidBody const& GetBtBody() const { return *this->_body; }
        World& GetWorld() { return this->_world; }

        void AddConstraint(Body* body);
        void RemoveConstraint(Body* body);

        void SetPhysics(std::vector<Node> const& physics);
        void SetAccel(btVector3 const& accel, btScalar maxSpeed);
        void SetLocalAccel(btVector3 const& accel, btScalar maxSpeed);

        std::vector<Common::Physics::Node> GetClusterPhysics() const;

        btVector3 const& GetAccel() const { return this->_acceleration; }
        bool IsAccelLocal() const { return this->_accelerationIsLocal; }
        btScalar GetMaxSpeed() const { return this->_maxSpeed; }

        void SetUserData(void* userData);
        void* GetUserData() { return this->_userData; }

        void Dump() const;
    };

}}

#endif
