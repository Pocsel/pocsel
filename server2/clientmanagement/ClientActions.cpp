#include "server2/clientmanagement/ClientActions.hpp"
#include "server2/clientmanagement/ClientManager.hpp"
#include "server2/clientmanagement/Client.hpp"

#include "protocol/protocol.hpp"

#include "common/Packet.hpp"

#include "server2/game/Game.hpp"
#include "server2/game/World.hpp"

#include "server2/game/map/Map.hpp"

#include "server2/database/ResourceManager.hpp"

#include "server2/network/PacketCreator.hpp"
#include "server2/network/PacketExtractor.hpp"

namespace Server { namespace ClientManagement {

    namespace ClientActionsNS { namespace {

        void _HandleLogin(ClientManager& manager, Client& client, Common::Packet const& packet)
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
                client.SendPacket(Network::PacketCreator::LoggedIn(false,
                    "Wrong login: cannot be empty"));
                return;
            }
            manager.ClientLogin(client, login);
        }

        void _HandlePong(ClientManager& manager, Client& client, Common::Packet const& packet)
        {
            Tools::debug << "_HandlePong (client " << client.id << ")\n";
        }

        void _HandleNeedChunks(ClientManager& manager, Client& client, Common::Packet const& packet)
        {
            Tools::debug << "_HandleNeedChunks (client " << client.id << ")\n";
            std::vector<Chunk::IdType> ids;
            Network::PacketExtractor::NeedChunks(packet, ids);

            Game::Map::Map::ChunkCallback callback = std::bind(&ClientManager::SendChunk, &manager, client.id, std::placeholders::_1);
            for (auto it = ids.begin(), ite = ids.end() ; it != ite ; ++it)
            {
                // TODO utiliser la bonne map
                manager.GetGame().GetWorld().GetDefaultMap()->GetChunk(*it, callback);
                //client.GetPlayer().GetMap().GetChunk(*it, callback);
            }
        }

        void _HandleGetNeededResourceIds(ClientManager& manager, Client& client, Common::Packet const& packet)
        {
            Tools::debug << "_HandleGetNeededResourceIds (client " << client.id << ")\n";
            Uint32 version;
            Network::PacketExtractor::GetNeededResourceIds(packet, version);

            std::vector<Uint32> ids(manager.GetResourceManager().GetNeededResourceIds(version));
            Uint32 offset = 0;
            do
            {
                Tools::debug << "ClientActions::_HandleGetNeededResourceIds: " << ids.size() << "\n";
                client.SendPacket(Network::PacketCreator::NeededResourceIds(ids, offset));
            } while (offset < ids.size());
        }

        void _HandleGetResourceRange(ClientManager& manager, Client& client, Common::Packet const& packet)
        {
            Tools::debug << "_HandleGetResourceRange (client " << client.id << ")\n";
            Uint32 id, offset;
            Network::PacketExtractor::GetResourceRange(packet, id, offset);

            Database::ResourceManager const& resourceManager = manager.GetResourceManager();
            if (!resourceManager.HasResource(id))
                throw std::runtime_error("Invalid resource id: " + Tools::ToString(id));
            Common::Resource const& resource = resourceManager.GetResource(id);
            client.SendPacket(Network::PacketCreator::ResourceRange(resource, offset));
        }

        void _HandleGetCubeType(ClientManager& manager, Client& client, Common::Packet const& packet)
        {
            Tools::debug << "_HandleGetCubeType (client " << client.id << ")\n";
            Chunk::CubeType id;
            Network::PacketExtractor::GetCubeType(packet, id);

            Game::World const& world = manager.GetGame().GetWorld();

            if (!world.HasCubeType(id))
                throw std::runtime_error("Invalid cube description id: " + Tools::ToString(id));
            client.SendPacket(Network::PacketCreator::CubeType(world.GetCubeType(id)));
        }

        // TODO rm OBSOLETE
        void _OBSOLETE(ClientManager&, Client& client, Common::Packet const&)
        {
            Tools::debug << "_OBSOLETE (client " << client.id << ")\n";
        }

        void _HandleSettings(ClientManager& manager, Client& client, Common::Packet const& packet)
        {
            Tools::debug << "_HandleSettings (client " << client.id << ")\n";

            Uint32 viewDistance;
            std::string playerName;

            Network::PacketExtractor::Settings(packet, viewDistance, playerName);

            // TODO client->SetSettings(viewDistance, playerName);

            manager.GetGame().GetWorld().GetDefaultMap()->GetSpawnPosition(
                    std::bind(&ClientManager::ClientTeleport, &manager, client.id, std::placeholders::_1)
                    );
        }

        void _HandleTeleportOk(ClientManager& manager, Client& client, Common::Packet const&)
        {
            Tools::debug << "_HandleTeleportOk (client " << client.id << ")\n";

            // TODO fin de cycle teleportation pour le client

        }

    }}

    void ClientActions::HandleAction(ClientManager& manager, Client& client, Common::Packet const& packet)
    {
        typedef void (*ActionCallback)(ClientManager&, Client&, Common::Packet const&);
        static ActionCallback actions[(int) Protocol::ClientToServer::NbPacketTypeClient] = {
            &ClientActionsNS::_HandleLogin,
            &ClientActionsNS::_HandlePong,
            &ClientActionsNS::_HandleNeedChunks,
            &ClientActionsNS::_HandleGetNeededResourceIds,
            &ClientActionsNS::_HandleGetResourceRange,
            &ClientActionsNS::_HandleGetCubeType,
            &ClientActionsNS::_OBSOLETE, // TODO rm
            &ClientActionsNS::_HandleSettings,
            &ClientActionsNS::_HandleTeleportOk,
        };

        static_assert((int) Protocol::ClientToServer::Login == 0, "wrong callback index");
        static_assert((int) Protocol::ClientToServer::Pong == 1, "wrong callback index");
        static_assert((int) Protocol::ClientToServer::NeedChunks == 2, "wrong callback index");
        static_assert((int) Protocol::ClientToServer::GetNeededResourceIds == 3, "wrong callback index");
        static_assert((int) Protocol::ClientToServer::GetResourceRange == 4, "wrong callback index");
        static_assert((int) Protocol::ClientToServer::GetCubeType == 5, "wrong callback index");
        static_assert((int) Protocol::ClientToServer::GetSpawnPosition == 6, "wrong callback index"); // TODO rm OBSOLETE
        static_assert((int) Protocol::ClientToServer::Settings == 7, "wrong callback index");
        static_assert((int) Protocol::ClientToServer::TeleportOk == 8, "wrong callback index");

        Protocol::ActionType action;
        packet.Read(action);

        if (action >= (sizeof(actions) / sizeof(*actions)))
            throw std::runtime_error("Unknown action: " + Tools::ToString<Uint32>(action));

#ifdef DEBUG
        if (actions[action] == 0)
            Tools::error << "Action not coded yet!\n";
#endif

        actions[action](manager, client, packet);
    }

}}
