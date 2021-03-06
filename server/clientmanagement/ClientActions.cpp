#include "server/precompiled.hpp"

#include "server/clientmanagement/ClientActions.hpp"
#include "server/clientmanagement/ClientManager.hpp"
#include "server/clientmanagement/Client.hpp"

#include "protocol/protocol.hpp"

#include "common/Packet.hpp"
#include "common/OrientedPosition.hpp"
#include "common/MovingOrientedPosition.hpp"
#include "common/CubePosition.hpp"

#include "server/Server.hpp"

#include "server/game/Game.hpp"
#include "server/game/PluginManager.hpp"
#include "server/game/World.hpp"
#include "server/game/map/Map.hpp"
#include "server/game/engine/Engine.hpp"
#include "server/game/engine/BodyManager.hpp"

#include "server/database/ResourceManager.hpp"

#include "server/network/PacketCreator.hpp"
#include "server/network/PacketExtractor.hpp"

#include "tools/ByteArray.hpp"

namespace Server { namespace ClientManagement {

    namespace ClientActionsNS { namespace {

        void _HandleLogin(ClientManager& manager, Client& client, Tools::ByteArray const& packet)
        {
            Tools::debug << "_HandleLogin (client " << client.id << ")\n";
            Protocol::Version major, minor;
            std::string login;
            Network::PacketExtractor::Login(packet, major, minor, login);
            if (major != Protocol::Version::Major || minor != Protocol::Version::Minor)
            {
                client.SendPacket(Network::PacketCreator::LoggedIn(false,
                    "Protocol verions mismatch : Server " + Tools::ToString(Protocol::Version::Major) +
                    "." + Tools::ToString(Protocol::Version::Minor) +
                    " != Client " + Tools::ToString(major) +
                    "." + Tools::ToString(minor)));
                return;
            }

            if (!login.size())
            {
                client.SendPacket(std::move(Network::PacketCreator::LoggedIn(false,
                    "Wrong login: cannot be empty")));
                return;
            }
            manager.ClientLogin(client, login);
        }

        void _HandlePong(ClientManager& manager, Client& client, Tools::ByteArray const& packet)
        {
            Tools::debug << "_HandlePong (client " << client.id << ")\n";
        }

        void _HandleNeedChunks(ClientManager& manager, Client& client, Tools::ByteArray const& packet)
        {
            Tools::debug << "_HandleNeedChunks (client " << client.id << ")\n";
            std::vector<Game::Map::Chunk::IdType> ids;
            Network::PacketExtractor::NeedChunks(packet, ids);

            manager.ClientNeedChunks(client, ids);
        }

        void _HandleGetNeededResourceIds(ClientManager& manager, Client& client, Tools::ByteArray const& packet)
        {
            Tools::debug << "_HandleGetNeededResourceIds (client " << client.id << ")\n";
            Uint32 version;
            Network::PacketExtractor::GetNeededResourceIds(packet, version);

            std::vector<Uint32> ids(manager.GetResourceManager().GetNeededResourceIds(version));
            Uint32 offset = 0;
            do
            {
                Tools::debug << "ClientActions::_HandleGetNeededResourceIds: " << ids.size() << "\n";
                client.SendPacket(std::move(Network::PacketCreator::NeededResourceIds(ids, offset)));
            } while (offset < ids.size());
        }

        void _HandleGetResourceRange(ClientManager& manager, Client& client, Tools::ByteArray const& packet)
        {
            Tools::debug << "_HandleGetResourceRange (client " << client.id << ")\n";
            Uint32 id, offset;
            Network::PacketExtractor::GetResourceRange(packet, id, offset);

            Database::ResourceManager const& resourceManager = manager.GetResourceManager();
            if (!resourceManager.HasResource(id))
                throw std::runtime_error("Invalid resource id: " + Tools::ToString(id));
            Common::Resource const& resource = resourceManager.GetResource(id);

            client.SendPacket(Network::PacketCreator::ResourceRange(manager.GetServer().GetGame().GetWorld().GetPluginManager(), resource, offset));
        }

        void _HandleGetBodyType(ClientManager& manager, Client& client, Tools::ByteArray const& packet)
        {
            Tools::debug << "_HandleGetCubeType (client " << client.id << ")\n";
            Uint32 id;
            Network::PacketExtractor::GetBodyType(packet, id);

            Game::Map::Map const& map = manager.GetServer().GetGame().GetWorld().GetDefaultMap();

            if (!map.GetEngine().GetBodyManager().HasBodyType(id))
                throw std::runtime_error("Invalid body type id: " + Tools::ToString(id));
            client.SendPacket(std::move(Network::PacketCreator::BodyType(map.GetEngine().GetBodyManager().GetBodyType(id))));
        }

        void _HandleGetCubeType(ClientManager& manager, Client& client, Tools::ByteArray const& packet)
        {
            Tools::debug << "_HandleGetCubeType (client " << client.id << ")\n";
            Game::Map::Chunk::CubeType id;
            Network::PacketExtractor::GetCubeType(packet, id);

            Game::Map::Map const& map = manager.GetServer().GetGame().GetWorld().GetDefaultMap();

            if (!map.HasCubeType(id))
                throw std::runtime_error("Invalid cube description id: " + Tools::ToString(id));
            client.SendPacket(std::move(Network::PacketCreator::CubeType(map.GetCubeType(id))));
        }

        void _HandleSettings(ClientManager& manager, Client& client, Tools::ByteArray const& packet)
        {
            Tools::debug << "_HandleSettings (client " << client.id << ")\n";

            Uint32 viewDistance;
            std::string playerName;

            Network::PacketExtractor::Settings(packet, viewDistance, playerName);

            if (playerName.size() == 0)
                playerName = "(void)";
//                throw std::runtime_error("Empty player name");

            manager.ClientSpawn(client, viewDistance, playerName);
        }

        void _HandleTeleportOk(ClientManager& manager, Client& client, Tools::ByteArray const&)
        {
            Tools::debug << "_HandleTeleportOk (client " << client.id << ")\n";

            manager.ClientTeleportOk(client);
        }

        void _HandleMove(ClientManager& manager, Client& client, Tools::ByteArray const& packet)
        {
//            Tools::debug << "_HandleMove (client " << client.id << ")\n";

            Common::MovingOrientedPosition pos;

            Network::PacketExtractor::Move(packet, pos);

            manager.GetServer().GetGame().PlayerMove(client.id, pos);
        }

        void _HandleAction(ClientManager& manager, Client& client, Tools::ByteArray const& packet)
        {
            Tools::debug << "_HandleAction (client " << client.id << ")\n";

            Common::OrientedPosition pos;
            Common::CubePosition cubePos;
            Uint32 actionId;

            Network::PacketExtractor::Action(packet, pos, cubePos, actionId);

            if (actionId == 1)
                manager.GetServer().GetGame().PlayerAction(client.id, pos, cubePos);
            else if (actionId == 2)
                manager.GetServer().GetGame().PlayerAction2(client.id, pos, cubePos);
            else
                Tools::log << "client " << client.id << ": unknown action " << actionId << ".\n";
        }

        void _HandleEntityRemoved(ClientManager& manager, Client& client, Tools::ByteArray const& packet)
        {
            Tools::debug << "_HandleDoodadRemoved (client " << client.id << ")\n";

            Uint32 entityId;
            Network::PacketExtractor::EntityRemoved(packet, entityId);

            manager.GetServer().GetGame().EntityRemovedForPlayer(client.id, entityId);
        }
    }}

    void ClientActions::HandleAction(ClientManager& manager, Client& client, Tools::ByteArray const& packet)
    {
        typedef void (*ActionCallback)(ClientManager&, Client&, Tools::ByteArray const&);
        static ActionCallback actions[] = {
            0, // UdpReady
            0, // ClPassThrough
            0, // ClPassThroughOk
            &ClientActionsNS::_HandleLogin,
            &ClientActionsNS::_HandlePong,
            &ClientActionsNS::_HandleNeedChunks,
            &ClientActionsNS::_HandleGetNeededResourceIds,
            &ClientActionsNS::_HandleGetResourceRange,
            &ClientActionsNS::_HandleGetCubeType,
            &ClientActionsNS::_HandleGetBodyType,
            &ClientActionsNS::_HandleSettings,
            &ClientActionsNS::_HandleTeleportOk,
            &ClientActionsNS::_HandleMove,
            &ClientActionsNS::_HandleAction,
            &ClientActionsNS::_HandleEntityRemoved
        };

        static_assert((int) Protocol::ClientToServer::Login == 3, "wrong callback index");
        static_assert((int) Protocol::ClientToServer::Pong == 4, "wrong callback index");
        static_assert((int) Protocol::ClientToServer::NeedChunks == 5, "wrong callback index");
        static_assert((int) Protocol::ClientToServer::GetNeededResourceIds == 6, "wrong callback index");
        static_assert((int) Protocol::ClientToServer::GetResourceRange == 7, "wrong callback index");
        static_assert((int) Protocol::ClientToServer::GetCubeType == 8, "wrong callback index");
        static_assert((int) Protocol::ClientToServer::GetBodyType == 9, "wrong callback index");
        static_assert((int) Protocol::ClientToServer::Settings == 10, "wrong callback index");
        static_assert((int) Protocol::ClientToServer::TeleportOk == 11, "wrong callback index");
        static_assert((int) Protocol::ClientToServer::Move == 12, "wrong callback index");
        static_assert((int) Protocol::ClientToServer::Action == 13, "wrong callback index");
        static_assert((int) Protocol::ClientToServer::EntityRemoved == 14, "wrong callback index");


        static_assert((size_t)Protocol::ClientToServer::NbPacketTypeClient == sizeof(actions) / sizeof(*actions),
                      "Not everything is implemented");

        Protocol::ActionType action;
        packet.Read(action);

        if (action >= (sizeof(actions) / sizeof(*actions)))
            throw std::runtime_error("Unknown action: " + Tools::ToString<Uint32>(action));

        assert(actions[action] != 0 && "action networkspecific ne doit pas se retrouver ici.");

        actions[action](manager, client, packet);
    }

}}
