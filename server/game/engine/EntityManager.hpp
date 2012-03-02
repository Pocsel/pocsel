#ifndef __SERVER_GAME_ENGINE_ENTITYMANAGER_HPP__
#define __SERVER_GAME_ENGINE_ENTITYMANAGER_HPP__

#include "tools/lua/Ref.hpp"

namespace Server { namespace Game { namespace Engine {

    class Engine;

    class EntityManager :
        private boost::noncopyable
    {
    private:
        struct EntityType
        {
            EntityType(std::string const& name, Uint32 pluginId, Tools::Lua::Ref const& type) :
                name(name), pluginId(pluginId), type(type)
            {
            }
            std::string name;
            Uint32 pluginId;
            Tools::Lua::Ref type;
        };
        struct Entity
        {
            Entity(EntityType* type, Tools::Lua::Ref const& self) :
                type(type), self(self)
            {
            }
            EntityType* type;
            Tools::Lua::Ref self;
        };

    private:
        Engine& _engine;
        std::map<Uint32, std::map<std::string, EntityType*>> _entityTypes;
        std::map<int, Entity*> _entities;
        Uint32 _pluginIdForRegistering;
        int _currentEntityId;

    public:
        EntityManager(Engine& engine);
        ~EntityManager();
        void BeginPluginRegistering(Uint32 pluginId);
        void EndPluginRegistering();
        void CallEntityFunction(int entityId, std::string function, Tools::Lua::Ref const& args);
        int SpawnEntity(std::string name, Uint32 pluginId, Tools::Lua::Ref const& args) throw(std::runtime_error);
        void SpawnInitEntities();
    private:
        void _Register(Tools::Lua::CallHelper& helper);
    };

}}}

#endif
