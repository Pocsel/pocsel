#include "server/game/engine/Doodad.hpp"
#include "server/game/engine/Engine.hpp"
#include "server/game/engine/PositionalEntity.hpp"
#include "server/network/PacketCreator.hpp"
#include "common/Packet.hpp"
#include "tools/lua/Interpreter.hpp"
#include "tools/lua/Iterator.hpp"
#include "tools/lua/Serializer.hpp"

namespace Server { namespace Game { namespace Engine {

    Doodad::Doodad(Engine& engine, Uint32 id, Uint32 pluginId, std::string const& name, Uint32 entityId, PositionalEntity const& entity) :
        _engine(engine), _id(id), _pluginId(pluginId), _name(name), _entityId(entityId), _entity(entity), _table(engine.GetInterpreter().MakeTable())
    {
        Tools::debug << "Doodad::Doodad: Doodad created (id " << this->_id << ", name \"" << this->_name << "\", pluginId " << this->_pluginId << ", entityId " << this->_entityId << ")." << std::endl;
    }

    Doodad::~Doodad()
    {
        Tools::debug << "Doodad::~Doodad: Doodad destroyed (id " << this->_id << ", name \"" << this->_name << "\", pluginId " << this->_pluginId << ", entityId " << this->_entityId << ")." << std::endl;
    }

    void Doodad::Disable()
    {
        // TODO
    }

    void Doodad::Enable()
    {
        // TODO
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
        // create packet
        Tools::Lua::Serializer const& serializer = this->_engine.GetInterpreter().GetSerializer();
        std::list<std::pair<std::string /* key */, std::string /* value */>> values;
        auto itTable = this->_table.Begin();
        auto itTableEnd = this->_table.End();
        for (; itTable != itTableEnd; ++itTable)
            values.push_back(std::make_pair(serializer.Serialize(itTable.GetKey(), true /* nilOnError */), serializer.Serialize(itTable.GetValue(), true /* nilOnError */)));
        auto packet = Network::PacketCreator::DoodadSpawn(this->_id, this->_pluginId, this->_name, this->_entity.GetPosition(), values);

        // send packet to new players
        auto it = this->_newPlayers.begin();
        auto itEnd = this->_newPlayers.end();
        for (; it != itEnd; ++it)
        {
            auto packetCopy = std::unique_ptr<Common::Packet>(new Common::Packet(*packet));
            this->_engine.SendPacket(*it, packetCopy);
            this->_players.insert(*it);
        }
        this->_newPlayers.clear();
    }

    void Doodad::ExecuteCommands()
    {
        this->_SpawnForNewPlayers();

        // create packet
        Tools::Lua::Serializer const& serializer = this->_engine.GetInterpreter().GetSerializer();
        std::list<std::tuple<bool /* functionCall */, std::string /* function || key */, std::string /* value */>> commands;
        while (!this->_commands.empty())
        {
            Command const& c = this->_commands.front();
            commands.push_back(std::make_tuple(c.functionCall, c.functionCall ? c.function : serializer.Serialize(c.key, true /* nilOnError */), serializer.Serialize(c.value, true /* nilOnError */)));
            if (!c.functionCall)
                this->_table.Set(c.key, c.value); // update of server state
            this->_commands.pop();
        }
        auto packet = Network::PacketCreator::DoodadUpdate(this->_id, &this->_entity.GetPosition(), commands);

        // send packet to players
        auto it = this->_players.begin();
        auto itEnd = this->_players.end();
        for (; it != itEnd; ++it)
        {
            auto packetCopy = std::unique_ptr<Common::Packet>(new Common::Packet(*packet));
            this->_engine.SendPacket(*it, packetCopy);
        }
    }

    void Doodad::Set(Tools::Lua::Ref const& key, Tools::Lua::Ref const& value)
    {
        Tools::Lua::Serializer const& serializer = this->_engine.GetInterpreter().GetSerializer();
        this->_commands.push(Command(serializer.MakeSerializableCopy(key, true /* nilOnError */), serializer.MakeSerializableCopy(value, true /* nilOnError */)));
    }

    void Doodad::Call(std::string const& name, Tools::Lua::Ref const& value)
    {
        Tools::Lua::Serializer const& serializer = this->_engine.GetInterpreter().GetSerializer();
        this->_commands.push(Command(name, serializer.MakeSerializableCopy(value, true /* nilOnError */)));
    }

}}}