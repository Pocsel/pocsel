#ifndef __SERVER_GAME_ENGINE_ENTITYMANAGER_HPP__
#define __SERVER_GAME_ENGINE_ENTITYMANAGER_HPP__

namespace Tools { namespace Lua {
    class Ref;
}}

namespace Server { namespace Game { namespace Engine {

    class Engine;

    class EntityManager
    {
    private:
        Engine& _engine;

    public:
        EntityManager(Engine& engine);
        ~EntityManager();
        void CallEntityFunction(int entityId, std::string function, Tools::Lua::Ref const& args);
    };

}}}

#endif
