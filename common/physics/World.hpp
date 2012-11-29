#ifndef __COMMON_PHYSICS_WORLD_HPP__
#define __COMMON_PHYSICS_WORLD_HPP__

#include "bullet/bullet-all.hpp"

namespace Common { namespace Physics {
    class Body;
    class BodyCluster;
}}

namespace Common { namespace Physics {

    class World
    {
    private:
        class CollisionFilter : public btCollisionDispatcher
        {
        private:
            World& _world;
        public:
            CollisionFilter(World& world, btDefaultCollisionConfiguration* colCfg);
        private:
            virtual bool needsCollision(btCollisionObject* body0, btCollisionObject* body1);
            virtual bool needsResponse(btCollisionObject* body0, btCollisionObject* body1);
            virtual void dispatchAllCollisionPairs(btOverlappingPairCache* pairCache, const btDispatcherInfo& dispatchInfo, btDispatcher* dispatcher);
        };
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
        std::vector<BodyCluster*> _bodyClusters;

        btVector3 _gravity;

    public:
        World();
        ~World();
        void Tick(Uint64 totalTime);

        btDynamicsWorld& GetBtWorld() { return *this->_dynamicsWorld; }

        size_t AddCallback(TickCallback cb, void* userPtr);
        void RemoveCallback(size_t idx);

        void AddBodyCluster(BodyCluster* body);
        void RemoveBodyCluster(BodyCluster* body);

        btVector3 const& GetGravity() const { return this->_gravity; }

    private:
        friend struct _cb;
        friend struct CollisionFilter;
    };

}}

#endif
