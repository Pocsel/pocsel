#ifndef __SERVER_GAME_ENGINE_DOODAD_HPP__
#define __SERVER_GAME_ENGINE_DOODAD_HPP__

#include "tools/lua/Ref.hpp"

namespace Server { namespace Game { namespace Engine {

    class PositionalEntity;

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
        Uint32 _id;
        Uint32 _pluginId;
        std::string _name;
        Uint32 _entityId;
        PositionalEntity const& _entity;
        Tools::Lua::Ref _table;
        std::unordered_set<Uint32> _players;
        std::unordered_set<Uint32> _newPlayers;
        std::queue<Command> _commands;

    public:
        Doodad(Uint32 id, Uint32 pluginId, std::string const& name, Uint32 entityId, PositionalEntity const& entity, Tools::Lua::Interpreter& interpreter);
        ~Doodad();
        Uint32 GetId() const { return this->_id; }
        Uint32 GetEntityId() const { return this->_entityId; }
        void Disable();
        void Enable();
        void AddPlayer(Uint32 playerId);
        void RemovePlayer(Uint32 playerId);
        void ExecuteCommands();
        void Set(Tools::Lua::Ref const& key, Tools::Lua::Ref const& value);
        void Call(std::string const& name, Tools::Lua::Ref const& value);
    private:
        void _SpawnForNewPlayers();
    };

}}}

#endif
