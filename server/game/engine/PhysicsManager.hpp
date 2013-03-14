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
        Common::Physics::World* _world;

        std::set<PositionalEntity*> _collidingEntities;
        std::set<PositionalEntity*> _newCollidingEntities;

    public:
        PhysicsManager(Engine& engine, std::map<Uint32, PositionalEntity*> const& entities);
        ~PhysicsManager();
        void Tick(Uint64 deltaTime);
        Common::Physics::World& GetWorld() { return *this->_world; }
        Common::Physics::World const& GetWorld() const { return *this->_world; }

    private:
        void _UpdateCollidingEntities();

        friend struct _cb;
    };

}}}

#endif
