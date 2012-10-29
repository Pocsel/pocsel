#ifndef __SERVER_GAME_ENGINE_DOODAD_HPP__
#define __SERVER_GAME_ENGINE_DOODAD_HPP__

#include "tools/lua/Ref.hpp"

namespace Common { namespace Physics {
    class World;
}}

namespace Server { namespace Game { namespace Engine {

    class PositionalEntity;
    class Engine;
    class Body;

    class Doodad :
        private boost::noncopyable
    {
    private:
        struct Command
        {
            Command(Tools::Lua::Ref const& key, Tools::Lua::Ref const& value) :
                functionCall(false), key(key), value(value)
            {
            }
            Command(std::string const& function, Tools::Lua::Ref const& value) :
                functionCall(true), function(function), key(value.GetState()) /* nil */, value(value)
            {
            }
            bool functionCall;
            std::string function;
            Tools::Lua::Ref key;
            Tools::Lua::Ref value;
        };

    private:
        Engine& _engine;
        Common::Physics::World& _world;
        Uint32 _id;
        Uint32 _pluginId;
        std::string _name;
        Uint32 _entityId;
        PositionalEntity& _entity;
        Tools::Lua::Ref _storage;
        std::unique_ptr<Body> _body;
        std::queue<Command> _commands;
        std::queue<Command> _commandsUdp;
        bool _positionDirty;
        bool _isNew;
        Uint32 _weakReferenceId;

    public:
        Doodad(Engine& engine,
                Uint32 id,
                Uint32 pluginId,
                std::string const& name,
                Uint32 entityId,
                PositionalEntity& entity,
                Body* body);
        ~Doodad();
        Uint32 GetId() const { return this->_id; }
        Uint32 GetPluginId() const { return this->_pluginId; }
        std::string const& GetName() const { return this->_name; }
        Uint32 GetEntityId() const { return this->_entityId; }
        Tools::Lua::Ref const& GetStorage() const { return this->_storage; }
        void SetStorage(Tools::Lua::Ref const& ref); // il faut pas que ça soit autre chose qu'une table sinon ça va throw grave
        void Disable();
        void Enable();
        void ExecuteCommands();
        void Set(Tools::Lua::Ref const& key, Tools::Lua::Ref const& value);
        void Call(std::string const& name, Tools::Lua::Ref const& value);
        void SetUdp(Tools::Lua::Ref const& key, Tools::Lua::Ref const& value);
        void CallUdp(std::string const& name, Tools::Lua::Ref const& value);
        void PositionIsDirty();
        Uint32 GetWeakReferenceId() const { return this->_weakReferenceId; }
    private:
        void _SpawnForNewPlayers();
    };

}}}

#endif
