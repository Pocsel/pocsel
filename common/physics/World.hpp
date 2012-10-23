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
    public:
        typedef void (*TickCallback)(void*);
    private:
        btDynamicsWorld* _dynamicsWorld;
        btBroadphaseInterface* _broadphase;
        btCollisionDispatcher* _dispatcher;
        btConstraintSolver* _solver;
        btDefaultCollisionConfiguration* _collisionConfiguration;

        Uint64 _lastTime;

        std::vector<std::pair<TickCallback, void*>> _callbacks;

    public:
        World();
        ~World();
        void Tick(Uint64 totalTime);

        btDynamicsWorld& GetBtWorld() { return *this->_dynamicsWorld; }

        size_t AddCallback(TickCallback cb, void* userPtr);
        void RemoveCallback(size_t idx);

    private:
        friend struct _cb;
    };

}}

#endif
