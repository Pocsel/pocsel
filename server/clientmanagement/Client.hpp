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
        bool teleportMode;
    private:
        boost::shared_ptr<Network::ClientConnection> _connection;
        std::string _login;
        std::string _playerName;

    public:
        Client(Uint32 id, boost::shared_ptr<Network::ClientConnection> connection);
        ~Client();

        void SendPacket(std::unique_ptr<Common::Packet> packet);
        void SendUdpPacket(std::unique_ptr<Common::Packet> packet);

        void SetLogin(std::string const& login);
        std::string const& GetLogin() const { return this->_login; }
        void SetPlayerName(std::string const& playerName);
        std::string const& GetPlayerName() const { return this->_playerName; }
    };

}}

#endif
