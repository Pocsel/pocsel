#ifndef __SERVER_GAME_ENGINE_PHYSICSMANAGER_HPP__
#define __SERVER_GAME_ENGINE_PHYSICSMANAGER_HPP__

namespace Common { namespace Physics {
    class World;
}}

namespace Server { namespace Game { namespace Engine {
    class PositionalEntity;
    class Engine;
}}}

class btRigidBody;

namespace Server { namespace Game { namespace Engine {

    class PhysicsManager
    {
    private:
        Engine& _engine;
        std::map<Uint32, PositionalEntity*> const& _entities;
        std::set<Uint32> _bodiesInWorld;
        std::set<btRigidBody const*> _entityBodies;
        Common::Physics::World* _world;

    public:
        PhysicsManager(Engine& engine, std::map<Uint32, PositionalEntity*> const& entities);
        ~PhysicsManager();
        void Tick(Uint64 deltaTime);

        friend struct _cb;
    };

}}}

#endif
