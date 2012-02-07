#ifndef __SERVER_CLIENTMANAGEMENT_CLIENT_HPP__
#define __SERVER_CLIENTMANAGEMENT_CLIENT_HPP__

namespace Common {

    class Packet;

}

namespace Server { namespace Network {

        class ClientConnection;

}}

namespace Server { namespace ClientManagement {

    class Client :
        private boost::noncopyable
    {
    public:
        Uint32 const id;
    private:
        boost::shared_ptr<Network::ClientConnection> _connection;

    public:
        Client(Uint32 id, Network::ClientConnection* connection);
        ~Client();

        void Shutdown();
        void SendPacket(Common::Packet* packet);
    };

}}

#endif
