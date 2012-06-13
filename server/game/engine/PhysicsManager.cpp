#include "server/game/engine/PhysicsManager.hpp"
#include "server/game/engine/PositionalEntity.hpp"
#include "server/game/engine/Engine.hpp"

#include "common/physics/Node.hpp"
#include "common/physics/Vector.hpp"
#include "common/physics/Move.hpp"

namespace Server { namespace Game { namespace Engine {

    PhysicsManager::PhysicsManager(Engine& engine, std::map<Uint32, PositionalEntity*> const& entities) :
        _engine(engine),
        _entities(entities)
    {
    }

    void PhysicsManager::Tick(Uint64 deltaTime)
    {
        double dt = deltaTime / (double)1000000.0;

        for (auto it = this->_entities.begin(), ite = this->_entities.end(); it != ite; ++it)
        {
            if (!it->second)
                continue;
            PositionalEntity& entity = *it->second;

            Common::Physics::MoveNode(entity.GetPhysics(), dt);
        }
    }

}}}
