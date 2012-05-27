#include "server/game/engine/Doodad.hpp"
#include "tools/lua/Interpreter.hpp"

namespace Server { namespace Game { namespace Engine {

    Doodad::Doodad(Uint32 id, Uint32 pluginId, std::string const& name, Uint32 entityId, PositionalEntity const& entity, Tools::Lua::Interpreter& interpreter) :
        _id(id), _pluginId(pluginId), _name(name), _entityId(entityId), _entity(entity), _table(interpreter.MakeTable())
    {
        Tools::debug << "Doodad::Doodad: Doodad created (id " << this->_id << ", name \"" << this->_name << "\", pluginId " << this->_pluginId << ", entityId " << this->_entityId << ")." << std::endl;
    }

    Doodad::~Doodad()
    {
        Tools::debug << "Doodad::~Doodad: Doodad destroyed (id " << this->_id << ", name \"" << this->_name << "\", pluginId " << this->_pluginId << ", entityId " << this->_entityId << ")." << std::endl;
    }

    void Doodad::Disable()
    {
    }

    void Doodad::Enable()
    {
    }

    void Doodad::AddPlayer(Uint32 playerId)
    {
        this->_newPlayers.insert(playerId);
    }

    void Doodad::RemovePlayer(Uint32 playerId)
    {
        this->_players.erase(playerId);
        this->_newPlayers.erase(playerId);
    }

    void Doodad::_SpawnForNewPlayers()
    {
        auto it = this->_newPlayers.begin();
        auto itEnd = this->_newPlayers.end();
        for (; it != itEnd; ++it)
        {
            auto itTable = this->_table.Begin();
            auto itTableEnd = this->_table.End();
            for (; itTable != itTableEnd; ++itTable)
            {
            }
            this->_players.insert(*it);
        }
        this->_newPlayers.clear();
    }

    void Doodad::ExecuteCommands()
    {
        this->_SpawnForNewPlayers();
    }

    void Doodad::Set(Tools::Lua::Ref const& key, Tools::Lua::Ref const& value)
    {
        this->_commands.push(Command(this->_table.GetState().GetInterpreter().GetSerializer().MakeSerializableCopy(key), this->_table.GetState().GetInterpreter().GetSerializer().MakeSerializableCopy(value)));
    }

    void Doodad::Call(std::string const& name, Tools::Lua::Ref const& value)
    {
        this->_commands.push(Command(name, this->_table.GetState().GetInterpreter().GetSerializer().MakeSerializableCopy(value)));
    }


}}}
