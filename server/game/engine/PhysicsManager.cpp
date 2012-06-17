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

        this->_world->GetBtWorld().stepSimulation(dt);

        for (auto it = this->_entities.begin(), ite = this->_entities.end(); it != ite; ++it)
        {
            if (!it->second)
                continue;

            PositionalEntity& entity = *it->second;
            btVector3 const& btPos = entity.GetBtBody().getCenterOfMassPosition();
            Common::Physics::Node& physics = entity.GetPhysics();

            physics.position.r.x = btPos.x();
            physics.position.r.y = btPos.y();
            physics.position.r.z = btPos.z();

            // std::cout << physics.position.r.x << ", ";
            // std::cout << physics.position.r.y << ", ";
            // std::cout << physics.position.r.z << "\n";

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
