#ifndef __SERVER_GAME_ENGINE_PHYSICSMANAGER_HPP__
#define __SERVER_GAME_ENGINE_PHYSICSMANAGER_HPP__

namespace Common { namespace Physics {
    struct Node;
    template<typename T> struct Vector;
}}

namespace Server { namespace Game { namespace Engine {
    class PositionalEntity;
    class Engine;
}}}

namespace Server { namespace Game { namespace Engine {

    class PhysicsManager
    {
    private:
        Engine& _engine;
        std::map<Uint32, PositionalEntity*> const& _entities;

    public:
        PhysicsManager(Engine& engine, std::map<Uint32, PositionalEntity*> const& entities);
        void Tick(Uint64 deltaTime);
    };

}}}

#endif
