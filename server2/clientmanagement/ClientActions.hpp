#ifndef __SERVER_CLIENTMANAGEMENT_CLIENTACTIONS_HPP__
#define __SERVER_CLIENTMANAGEMENT_CLIENTACTIONS_HPP__

namespace Common {

    class Packet;

}

namespace Server { namespace ClientManagement {

    class ClientManager;
    class Client;

    class ClientActions
    {
    public:
        static void HandleAction(ClientManager& manager, Client& client, Common::Packet& packet);

    private:
        static void _HandleLogin(ClientManager& manager, Client& client, Common::Packet& packet);
        static void _HandlePong(ClientManager& manager, Client& client, Common::Packet& packet);
        static void _HandleNeedChunks(ClientManager& manager, Client& client, Common::Packet& packet);
        static void _HandleGetNeededResourceIds(ClientManager& manager, Client& client, Common::Packet& packet);
        static void _HandleGetResourceRange(ClientManager& manager, Client& client, Common::Packet& packet);
        static void _HandleGetCubeType(ClientManager& manager, Client& client, Common::Packet& packet);
        static void _HandleGetSpawnPosition(ClientManager& manager, Client& client, Common::Packet& packet);
    };

}}

#endif
