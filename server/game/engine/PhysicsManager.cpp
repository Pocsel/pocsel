#include "server/game/engine/PhysicsManager.hpp"
#include "server/game/engine/PositionalEntity.hpp"
#include "server/game/engine/Engine.hpp"

#include "common/physics/Node.hpp"
#include "common/physics/Vector.hpp"
#include "common/physics/Move.hpp"
#include "common/physics/World.hpp"

#include "bullet/bullet-all.hpp"

namespace Server { namespace Game { namespace Engine {

    PhysicsManager::PhysicsManager(Engine& engine, std::map<Uint32, PositionalEntity*> const& entities) :
        _engine(engine),
        _entities(entities),
        _world(0)
    {
        this->_world = new Common::Physics::World();
    }

    PhysicsManager::~PhysicsManager()
    {
        Tools::Delete(this->_world);
    }

    void PhysicsManager::Tick(Uint64 deltaTime)
    {
        for (auto it = this->_entities.begin(), ite = this->_entities.end(); it != ite; ++it)
        {
            if (!this->_bodiesInWorld.count(it->first))
            {
                this->_world->GetBtWorld().addRigidBody(&it->second->GetBtBody());
                this->_bodiesInWorld.insert(it->first);
            }
        }

        double dt = deltaTime / (double)1000000.0;

        this->_world->Tick(deltaTime);

        for (auto it = this->_entities.begin(), ite = this->_entities.end(); it != ite; ++it)
        {
            if (!it->second)
                continue;

            PositionalEntity& entity = *it->second;
            btVector3 const& btPos = entity.GetBtBody().getCenterOfMassPosition();
            btVector3 const& btVel = entity.GetBtBody().getLinearVelocity();
            Common::Physics::Node& physics = entity.GetPhysics();

            btTransform wt;
            entity.GetBtBody().getMotionState()->getWorldTransform(wt);
            btVector3 wpos = wt.getOrigin();

            physics.position.r.x = wpos.x();//BtPos.x();
            physics.position.r.y = wpos.y();//BtPos.y();
            physics.position.r.z = wpos.z();//BtPos.z();

            physics.position.v.x = btVel.x();
            physics.position.v.y = btVel.y();
            physics.position.v.z = btVel.z();


            // std::cout << physics.position.v.x << ", ";
            // std::cout << physics.position.v.y << ", ";
            // std::cout << physics.position.v.z << "\n";

            // TODO j'en suis là

            // Common::Physics::MoveNode(entity.GetPhysics(), dt);
        }

        //for (auto it = this->_entities.begin(), ite = this->_entities.end(); it != ite; ++it)
        //{
        //    if (!it->second)
        //        continue;
        //    PositionalEntity& entity = *it->second;

        //    Common::Physics::MoveNode(entity.GetPhysics(), dt);
        //}
    }

}}}
