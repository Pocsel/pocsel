#include "common/physics/World.hpp"
#include "common/physics/Body.hpp"

#include "bullet/bullet-all.hpp"


namespace Common { namespace Physics {

    struct _cb {
        static void _TickCallBack(btDynamicsWorld* btWorld, btScalar timeStep)
        {
            World& world = *(World*)btWorld->getWorldUserInfo();
            for (auto cb: world._callbacks)
            {
                if (cb.first)
                    cb.first(cb.second);
            }
        }
    };

    World::World() :
        _dynamicsWorld(0),
        _broadphase(0),
        _dispatcher(0),
        _solver(0),
        _collisionConfiguration(0),
        _lastTime(0)
    {
        ///collision configuration contains default setup for memory, collision setup
        this->_collisionConfiguration = new btDefaultCollisionConfiguration();
        ///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
        this->_dispatcher = new btCollisionDispatcher(_collisionConfiguration);

        this->_broadphase = new btDbvtBroadphase();

        ///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
        this->_solver = new btSequentialImpulseConstraintSolver();

        this->_dynamicsWorld = new btDiscreteDynamicsWorld(_dispatcher, _broadphase, _solver, _collisionConfiguration);

        this->_dynamicsWorld->setGravity(btVector3(0.0,-9.81,0.0));

        this->_dynamicsWorld->setInternalTickCallback(_cb::_TickCallBack, this);
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
        double deltaTime = (totalTime - this->_lastTime) * 0.000001;
        this->_dynamicsWorld->stepSimulation(deltaTime, 4, 1.0 / 60.0);
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

}}
