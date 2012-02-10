#include <iostream>
#include <boost/bind.hpp>

#include "server/Game.hpp"
#include "server/ClientActions.hpp"
#include "server/PacketCreator.hpp"
#include "server/PacketExtractor.hpp"
#include "server/Chunk.hpp"
#include "server/Client.hpp"
#include "server/ClientManager.hpp"
#include "server/Map.hpp"
#include "server/World.hpp"

#include "tools/ToString.hpp"

#include "common/Packet.hpp"

#include "protocol/protocol.hpp"

namespace Server {

    void ClientActions::HandleAction(Game& game, ClientPtr& client, Common::Packet& packet)
    {
        typedef void (*ActionCallback)(Game&, ClientPtr&, Common::Packet&);
        static ActionCallback actions[(int) Protocol::ClientToServer::NbPacketTypeClient] = {
            &ClientActions::_HandleLogin,
            &ClientActions::_HandlePong,
            &ClientActions::_HandleNeedChunks,
            &ClientActions::_HandleGetNeededResourceIds,
            &ClientActions::_HandleGetResourceRange,
            &ClientActions::_HandleGetCubeType,
            &ClientActions::_HandleGetSpawnPosition,
        };

        static_assert((int) Protocol::ClientToServer::Login == 0, "wrong callback index");
        static_assert((int) Protocol::ClientToServer::Pong == 1, "wrong callback index");
        static_assert((int) Protocol::ClientToServer::NeedChunks == 2, "wrong callback index");
        static_assert((int) Protocol::ClientToServer::GetNeededResourceIds == 3, "wrong callback index");
        static_assert((int) Protocol::ClientToServer::GetResourceRange == 4, "wrong callback index");
        static_assert((int) Protocol::ClientToServer::GetCubeType == 5, "wrong callback index");
        static_assert((int) Protocol::ClientToServer::GetSpawnPosition == 6, "wrong callback index");

        static_assert(sizeof(Protocol::ActionType) == 1, "faut changer le packet.Read8()");
        Protocol::ActionType action = packet.Read8();

        if (action >= (sizeof(actions) / sizeof(*actions)))
            throw std::runtime_error("Unknown action: " + Tools::ToString<Uint32>(action));
        assert(actions[action] && "Action not coded yet");
        actions[action](game, client, packet);
    }

    void ClientActions::_HandleLogin(Game& game, ClientPtr& client, Common::Packet& packet)
    {
        Protocol::Version major, minor;
        std::string login;
        PacketExtractor::Login(packet, major, minor, login);
        if (major != Protocol::Version::Major || minor != Protocol::Version::Minor)
            throw std::runtime_error(
                "Protocol verions mismatch : Server " +
                Tools::ToString(Protocol::Version::Major) + "." + Tools::ToString(Protocol::Version::Minor)
                + " != Client " +
                Tools::ToString(major) + "." + Tools::ToString(minor)
            );
        if (!login.size())
            throw std::runtime_error("Wrong login: cannot be empty");
        game.GetClientManager().ClientLogin(client, login);
    }

    void ClientActions::_HandlePong(Game& game, ClientPtr& client, Common::Packet& packet)
    {
    }

    void ClientActions::_HandleNeedChunks(Game&, ClientPtr& client, Common::Packet& packet)
    {
        std::list<Chunk::IdType> ids;
        PacketExtractor::NeedChunks(packet, ids);
        std::cout << "Need Chunks:";
        Map::RequestCallback callback = std::bind(&Client::SendChunk, client, std::placeholders::_1);
        for (auto it = ids.begin(), ite = ids.end() ; it != ite ; ++it)
        {
            std::cout << ' ' << *it;
            client->GetPlayer().GetMap().GetChunk(*it, callback);
        }
        std::cout << std::endl;
    }

    void ClientActions::_HandleGetNeededResourceIds(Game& game, ClientPtr& client, Common::Packet& packet)
    {
        Uint32 version;
        packet.Read(version);

        std::vector<Uint32> ids(game.GetResourceManager().GetNeededResourceIds(version)); // C++0x, ça move
        Uint32 offset = 0;
        do
        {
            std::cout << "ClientActions::_HandleGetNeededResourceIds: " << ids.size() << std::endl;
            client->SendPacket(PacketCreator::NeededResourceIdsPacket(ids, offset));
        } while (offset < ids.size());
    }

    void ClientActions::_HandleGetResourceRange(Game& game, ClientPtr& client, Common::Packet& packet)
    {
        Uint32 id, offset;
        packet.Read(id);
        packet.Read(offset);
        auto& resourceManager = game.GetWorld().GetResourceManager();
        if (!resourceManager.HasResource(id))
            throw std::runtime_error("Invalid resource id: " + Tools::ToString(id));
        auto& resource = resourceManager.GetResource(id);
        auto response = PacketCreator::ResourceRangePacket(resource, offset);
        client->SendPacket(std::move(response));
    }

    void ClientActions::_HandleGetCubeType(Game& game, ClientPtr& client, Common::Packet& packet)
    {
        Chunk::CubeType id;
        PacketExtractor::ExtractGetCubeType(packet, id);
        if (!game.GetWorld().HasCubeType(id))
            throw std::runtime_error("Invalid cube description id: " + Tools::ToString(id));
        auto& cubeType = game.GetWorld().GetCubeType(id);
        auto response = PacketCreator::CubeTypePacket(cubeType);
        client->SendPacket(std::move(response));
    }

    void ClientActions::_HandleGetSpawnPosition(Game& game, ClientPtr& client, Common::Packet&)
    {
        std::cout << "_HandleGetSpawnPosition\n";

        game.GetWorld().GetDefaultMap().GetSpawnPosition(
                std::bind(&Client::TeleportPlayer, client, std::placeholders::_1)
                );
    }

}
