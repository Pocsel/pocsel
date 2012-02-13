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
        static void HandleAction(ClientManager& manager, Client& client, Common::Packet const& packet);
    };

}}

#endif
