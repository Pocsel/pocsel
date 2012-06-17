#ifndef __COMMON_PHYSICS_WORLD_HPP__
#define __COMMON_PHYSICS_WORLD_HPP__

class btDynamicsWorld;
class btBroadphaseInterface;
class btCollisionDispatcher;
class btConstraintSolver;
class btDefaultCollisionConfiguration;

namespace Common { namespace Physics {
    class Body;
}}

namespace Common { namespace Physics {

    class World
    {
    private:
        btDynamicsWorld* _dynamicsWorld;
        btBroadphaseInterface* _broadphase;
        btCollisionDispatcher* _dispatcher;
        btConstraintSolver* _solver;
        btDefaultCollisionConfiguration* _collisionConfiguration;

        Uint64 _lastTime;

    public:
        World();
        ~World();
        void Tick(Uint64 totalTime);
        void AddBody(Body& body);

        btDynamicsWorld& GetBtWorld() { return *this->_dynamicsWorld; }
    };

}}

#endif
