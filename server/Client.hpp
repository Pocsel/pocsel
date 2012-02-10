#ifndef __SERVER_CLIENT_HPP__
#define __SERVER_CLIENT_HPP__

#include <boost/noncopyable.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/function.hpp>

#include "tools/MessageQueue.hpp"

#include "common/Packet.hpp"
#include "common/PacketPtr.hpp"

#include "server/SocketPtr.hpp"
#include "server/ClientPtr.hpp"
#include "server/ClientConnection.hpp"
#include "server/Player.hpp"

namespace Common {
    struct Position;
}

namespace Server {

    struct Chunk;

    class Client :
        public Tools::MessageQueue<Client>,
        public boost::enable_shared_from_this<Client>,
        private boost::noncopyable
    {
    public:
        typedef boost::function<void(ClientPtr&, Common::PacketPtr&)> HandlePacketCallback;
        typedef boost::function<void(ClientPtr&, boost::system::error_code const&)> HandleErrorCallback;

    private:
        boost::shared_ptr<ClientConnection>     _conn;
        std::string                             _login;
        HandlePacketCallback                    _onPacket;
        HandleErrorCallback                     _onError;
        Player*                                 _player;

    public:
        Client(HandlePacketCallback& onPacket, HandleErrorCallback& onError);
        ~Client();
        void Connect(SocketPtr& socket);
        void SendChunk(Chunk const& chunk);
        void Shutdown();
        void Login(std::string const& login);
        void Logout();
        std::string const& GetLogin() const { return this->_login; }
        bool IsLoggedIn() const;

        Player& GetPlayer()
        {
            if (!this->_player)
                throw std::runtime_error("The client does not have a player for now");
            return *this->_player;
        }

        void SendPacket(Common::PacketPtr packet)
        {
            if (!this->_conn)
                return;
            this->_conn->SendPacket(std::move(packet));
        }

        void Spawn(Map& map)
        {
            if (this->_player)
                throw std::runtime_error("Player already spawned");
            std::cout << "SPAWNING PLAYER ! (fake)\n";
            this->_player = new Player(map);
        }

        void TeleportPlayer(Common::Position const& pos);
    };

}

#endif
