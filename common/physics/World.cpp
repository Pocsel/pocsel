#include "common/physics/World.hpp"
#include "common/physics/Body.hpp"

#include "bullet/bullet-all.hpp"


namespace Common { namespace Physics {

    World::World() :
        _dynamicsWorld(0),
        _broadphase(0),
        _dispatcher(0),
        _solver(0),
        _collisionConfiguration(0),
        _lastTime(0)
    {
        ///collision configuration contains default setup for memory, collision setup
        _collisionConfiguration = new btDefaultCollisionConfiguration();
        ///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
        _dispatcher = new btCollisionDispatcher(_collisionConfiguration);

        _broadphase = new btDbvtBroadphase();

        ///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
        _solver = new btSequentialImpulseConstraintSolver();

        _dynamicsWorld = new btDiscreteDynamicsWorld(_dispatcher, _broadphase, _solver, _collisionConfiguration);

        _dynamicsWorld->setGravity(btVector3(0,-10,0));
    }

    World::~World()
    {
        Tools::Delete(this->_dynamicsWorld);
        Tools::Delete(this->_broadphase);
        Tools::Delete(this->_dispatcher);
        Tools::Delete(this->_solver);
        Tools::Delete(this->_collisionConfiguration);
    }

    void World::Tick(Uint64 totalTime)
    {
        double deltaTime = (totalTime - this->_lastTime) * 0.000001;
        this->_dynamicsWorld->stepSimulation(deltaTime);
    }

}}
