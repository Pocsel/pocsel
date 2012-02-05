#ifndef __CLIENTACTIONS_HPP__
#define __CLIENTACTIONS_HPP__

#include "common/PacketPtr.hpp"

#include "server/ClientPtr.hpp"
#include "server/Chunk.hpp"

namespace Server {

    class Client;
    class Game;

    class ClientActions
    {
    public:
        static void HandleAction(Game& game, ClientPtr& client, Common::Packet& packet);

    private:
        static void _HandleLogin(Game& game, ClientPtr& client, Common::Packet& packet);
        static void _HandlePong(Game& game, ClientPtr& client, Common::Packet& packet);
        static void _HandleNeedChunks(Game& game, ClientPtr& client, Common::Packet& packet);
        static void _HandleGetNeededResourceIds(Game& game, ClientPtr& client, Common::Packet& packet);
        static void _HandleGetResourceRange(Game& game, ClientPtr& client, Common::Packet& packet);
        static void _HandleGetCubeType(Game& game, ClientPtr& client, Common::Packet& packet);
        static void _HandleGetSpawnPosition(Game& game, ClientPtr& client, Common::Packet& packet);
    };
}

#endif
