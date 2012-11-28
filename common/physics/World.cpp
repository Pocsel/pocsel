#include "common/physics/World.hpp"
#include "common/physics/Body.hpp"
#include "common/physics/BodyCluster.hpp"

#include "bullet/bullet-all.hpp"


namespace Common { namespace Physics {

    World::CollisionFilter::CollisionFilter(World& world, btDefaultCollisionConfiguration* colCfg) :
        btCollisionDispatcher(colCfg),
        _world(world)
    {
    }

    bool World::CollisionFilter::needsCollision(
            btCollisionObject* body0,
            btCollisionObject* body1)
    {
        if (body0->getUserPointer() == body1->getUserPointer())
            return false;
        return this->btCollisionDispatcher::needsCollision(body0, body1);
    }
    bool World::CollisionFilter::needsResponse(
            btCollisionObject* body0,
            btCollisionObject* body1)
    {
        return this->btCollisionDispatcher::needsResponse(body0, body1);
    }
    void World::CollisionFilter::dispatchAllCollisionPairs(
            btOverlappingPairCache* pairCache,
            const btDispatcherInfo& dispatchInfo,
            btDispatcher* dispatcher)
    {
        return this->btCollisionDispatcher::dispatchAllCollisionPairs(pairCache, dispatchInfo, dispatcher);
    }

    struct _cb {
        static void _TickCallBack(btDynamicsWorld* btWorld, btScalar timeStep)
        {
            World& world = *(World*)btWorld->getWorldUserInfo();
            for (auto cb: world._callbacks)
            {
                if (cb.first)
                    cb.first(cb.second);
            }

            //for (auto body: world._bodyClusters)
            //{
            //    body->Tick();
            //}
        }
        static void _PreTickCallBack(btDynamicsWorld* btWorld, btScalar timeStep)
        {
            World& world = *(World*)btWorld->getWorldUserInfo();
            //for (auto cb: world._callbacks)
            //{
            //    if (cb.first)
            //        cb.first(cb.second);
            //}

            for (auto body: world._bodyClusters)
            {
                body->PreBtTick(timeStep);
            }
        }
    };

    World::World() :
        _dynamicsWorld(0),
        _broadphase(0),
        _dispatcher(0),
        _solver(0),
        _collisionConfiguration(0),
        _lastTime(0),
        _gravity(0.0,-9.81,0.0)
    {
        ///collision configuration contains default setup for memory, collision setup
        this->_collisionConfiguration = new btDefaultCollisionConfiguration();
        ///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
        this->_dispatcher = new CollisionFilter(*this, this->_collisionConfiguration);

        this->_broadphase = new btDbvtBroadphase();

        ///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
        this->_solver = new btSequentialImpulseConstraintSolver();

        this->_dynamicsWorld = new btDiscreteDynamicsWorld(_dispatcher, _broadphase, _solver, _collisionConfiguration);

        this->_dynamicsWorld->setGravity(this->_gravity);

        this->_dynamicsWorld->setInternalTickCallback(_cb::_TickCallBack, this);
        this->_dynamicsWorld->setInternalTickCallback(_cb::_PreTickCallBack, this, true);
    }

    World::~World()
    {
        Tools::Delete(this->_dynamicsWorld);
        Tools::Delete(this->_solver);
        Tools::Delete(this->_broadphase);
        Tools::Delete(this->_dispatcher);
        Tools::Delete(this->_collisionConfiguration);
    }

    void World::Tick(Uint64 totalTime)
    {
        //std::cout << "TICK\n";
        for (auto body: this->_bodyClusters)
        {
            body->PreTick();
        }

        double deltaTime = (totalTime - this->_lastTime) * 0.000001;
        this->_dynamicsWorld->stepSimulation(deltaTime, 10, 1.0 / 60.0);

        for (auto body: this->_bodyClusters)
        {
            body->PostTick();
        }
    }

    size_t World::AddCallback(TickCallback cb, void* userPtr)
    {
        this->_callbacks.push_back(std::make_pair(cb, userPtr));
        return this->_callbacks.size() - 1;
    }

    void World::RemoveCallback(size_t idx)
    {
        assert(this->_callbacks.size() > idx);
        this->_callbacks[idx].first = 0;
    }

    void World::AddBodyCluster(BodyCluster* body)
    {
        this->_bodyClusters.push_back(body);
    }

    void World::RemoveBodyCluster(BodyCluster* body)
    {
        for (auto it = this->_bodyClusters.begin(), ite = this->_bodyClusters.end(); it != ite; ++it)
        {
            if (*it == body)
            {
                this->_bodyClusters.erase(it);
                return;
            }
        }
        assert(true && "this cluster has not been found");
    }

}}
