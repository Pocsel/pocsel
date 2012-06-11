#include "server/precompiled.hpp"

#include "server/game/PluginManager.hpp"
#include "server/game/World.hpp"
#include "server/game/engine/Doodad.hpp"
#include "server/game/engine/DoodadManager.hpp"
#include "server/game/engine/Engine.hpp"
#include "server/game/engine/PositionalEntity.hpp"
#include "server/game/engine/Body.hpp"
#include "server/game/map/Map.hpp"
#include "server/network/PacketCreator.hpp"
#include "server/network/UdpPacket.hpp"
#include "common/FieldUtils.hpp"
#include "common/Packet.hpp"
#include "tools/lua/Interpreter.hpp"
#include "tools/lua/Iterator.hpp"
#include "tools/lua/Serializer.hpp"

namespace Server { namespace Game { namespace Engine {

    Doodad::Doodad(Engine& engine, Uint32 id, Uint32 pluginId, std::string const& name, Uint32 entityId, PositionalEntity const& entity, Body* body) :
        _engine(engine),
        _id(id),
        _pluginId(pluginId),
        _name(name),
        _entityId(entityId),
        _entity(entity),
        _storage(engine.GetInterpreter().MakeTable()),
        _body(body),
        _positionDirty(false)
    {
        Tools::debug << "Doodad::Doodad: Doodad created (id " << this->_id << ", name \"" << this->_name << "\", pluginId " << this->_pluginId << ", entityId " << this->_entityId << ")." << std::endl;
    }

    Doodad::~Doodad()
    {
        Tools::debug << "Doodad::~Doodad: Doodad destroyed (id " << this->_id << ", name \"" << this->_name << "\", pluginId " << this->_pluginId << ", entityId " << this->_entityId << ")." << std::endl;

        // create kill packet
        auto packet = Network::PacketCreator::DoodadKill(this->_id);

        // send kill packet to players
        auto it = this->_players.begin();
        auto itEnd = this->_players.end();
        for (; it != itEnd; ++it)
        {
            auto packetCopy = std::unique_ptr<Common::Packet>(new Common::Packet(*packet));
            this->_engine.SendPacket(*it, packetCopy);
        }

        // sécurité en plus, je pense que ça sert a rien
        this->_engine.GetDoodadManager().DoodadIsClean(this);
    }

    void Doodad::Disable()
    {
        // TODO
    }

    void Doodad::Enable()
    {
        // TODO
    }

    void Doodad::SetStorage(Tools::Lua::Ref const& ref)
    {
        this->_storage = ref;
    }

    void Doodad::AddPlayer(Uint32 playerId)
    {
        this->_newPlayers.insert(playerId);
        this->_engine.GetDoodadManager().DoodadIsDirty(this);
    }

    void Doodad::RemovePlayer(Uint32 playerId)
    {
        this->_players.erase(playerId);
        this->_newPlayers.erase(playerId);
    }

    void Doodad::_SpawnForNewPlayers()
    {
        if (this->_newPlayers.empty())
            return;

        // create packet
        Tools::Lua::Serializer const& serializer = this->_engine.GetInterpreter().GetSerializer();
        std::list<std::pair<std::string /* key */, std::string /* value */>> values;
        auto itTable = this->_storage.Begin();
        auto itTableEnd = this->_storage.End();
        for (; itTable != itTableEnd; ++itTable)
            values.push_back(std::make_pair(serializer.Serialize(itTable.GetKey(), true /* nilOnError */), serializer.Serialize(itTable.GetValue(), true /* nilOnError */)));
        auto const& pluginName = this->_engine.GetWorld().GetPluginManager().GetPluginIdentifier(this->_pluginId);
        auto packet = Network::PacketCreator::DoodadSpawn(this->_id, Common::FieldUtils::GetResourceName(pluginName, this->_name), this->_entity.GetPosition(), values);

        // send packet to new players
        auto it = this->_newPlayers.begin();
        auto itEnd = this->_newPlayers.end();
        for (; it != itEnd; ++it)
            if (this->_engine.GetMap().HasPlayer(*it))
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

        if (this->_commands.empty() && this->_commandsUdp.empty() && !this->_positionDirty)
            return;

        // create packet
        Tools::Lua::Serializer const& serializer = this->_engine.GetInterpreter().GetSerializer();
        std::list<std::tuple<bool /* functionCall */, std::string /* function || key */, std::string /* value */>> commands;
        bool tcp = false;
        while (!this->_commands.empty())
        {
            Command const& c = this->_commands.front();
            commands.push_back(std::make_tuple(c.functionCall, c.functionCall ? c.function : serializer.Serialize(c.key, true /* nilOnError */), serializer.Serialize(c.value, true /* nilOnError */)));
            if (!c.functionCall)
                this->_storage.Set(c.key, c.value); // update of server state
            this->_commands.pop();
            tcp = true;
        }
        while (!this->_commandsUdp.empty())
        {
            Command const& c = this->_commands.front();
            commands.push_back(std::make_tuple(c.functionCall, c.functionCall ? c.function : serializer.Serialize(c.key, true /* nilOnError */), serializer.Serialize(c.value, true /* nilOnError */)));
            if (!c.functionCall)
                this->_storage.Set(c.key, c.value); // update of server state
            this->_commands.pop();
        }
        auto packet = Network::PacketCreator::DoodadUpdate(this->_id, this->_positionDirty ? &this->_entity.GetPosition() : 0, commands);
        this->_positionDirty = false;

        // send packet to players
        auto it = this->_players.begin();
        auto itEnd = this->_players.end();
        while (it != itEnd)
            if (this->_engine.GetMap().HasPlayer(*it))
            {
                if (tcp)
                {
                    auto packetCopy = std::unique_ptr<Common::Packet>(new Common::Packet(*packet));
                    this->_engine.SendPacket(*it, packetCopy);
                }
                else
                {
                    auto packetCopy = std::unique_ptr<Network::UdpPacket>(new Network::UdpPacket(*packet));
                    this->_engine.SendUdpPacket(*it, packetCopy);
                }
                ++it;
            }
            else
                this->_players.erase(it++);
    }

    void Doodad::Set(Tools::Lua::Ref const& key, Tools::Lua::Ref const& value)
    {
        Tools::Lua::Serializer const& serializer = this->_engine.GetInterpreter().GetSerializer();
        this->_commands.push(Command(serializer.MakeSerializableCopy(key, true /* nilOnError */), serializer.MakeSerializableCopy(value, true /* nilOnError */)));
        this->_engine.GetDoodadManager().DoodadIsDirty(this);
    }

    void Doodad::Call(std::string const& name, Tools::Lua::Ref const& value)
    {
        Tools::Lua::Serializer const& serializer = this->_engine.GetInterpreter().GetSerializer();
        this->_commands.push(Command(name, serializer.MakeSerializableCopy(value, true /* nilOnError */)));
        this->_engine.GetDoodadManager().DoodadIsDirty(this);
    }

    void Doodad::SetUdp(Tools::Lua::Ref const& key, Tools::Lua::Ref const& value)
    {
        Tools::Lua::Serializer const& serializer = this->_engine.GetInterpreter().GetSerializer();
        this->_commandsUdp.push(Command(serializer.MakeSerializableCopy(key, true /* nilOnError */), serializer.MakeSerializableCopy(value, true /* nilOnError */)));
        this->_engine.GetDoodadManager().DoodadIsDirty(this);
    }

    void Doodad::CallUdp(std::string const& name, Tools::Lua::Ref const& value)
    {
        Tools::Lua::Serializer const& serializer = this->_engine.GetInterpreter().GetSerializer();
        this->_commandsUdp.push(Command(name, serializer.MakeSerializableCopy(value, true /* nilOnError */)));
        this->_engine.GetDoodadManager().DoodadIsDirty(this);
    }

    void Doodad::PositionIsDirty()
    {
        this->_positionDirty = true;
        this->_engine.GetDoodadManager().DoodadIsDirty(this);
    }

}}}
