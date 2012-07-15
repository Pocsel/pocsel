#ifndef __CLIENT_GAME_ENGINE_PHYSICSMANAGER_HPP__
#define __CLIENT_GAME_ENGINE_PHYSICSMANAGER_HPP__

namespace Common { namespace Physics {
    class World;
}}

namespace Client { namespace Game {
    class BulletDebugDrawer;
}}

class btRigidBody;

namespace Client { namespace Game { namespace Engine {

    class Doodad;
    class Engine;

    class PhysicsManager
    {
    private:
        Engine& _engine;
        std::map<Uint32, Doodad*> const& _doodads;
        std::set<Uint32> _bodiesInWorld;
        std::set<btRigidBody const*> _doodadBodies;
        Common::Physics::World* _world;
        Uint64 _lastTime;

        BulletDebugDrawer* _debugDrawer;

    public:
        PhysicsManager(Engine& engine, std::map<Uint32, Doodad*> const& entities);
        ~PhysicsManager();
        void Tick(Uint64 deltaTime);

        Common::Physics::World& GetWorld() { return *this->_world; }
        Common::Physics::World const& GetWorld() const { return *this->_world; }

        BulletDebugDrawer& GetDebugDrawer() { return *this->_debugDrawer; }
    };

}}}

#endif
