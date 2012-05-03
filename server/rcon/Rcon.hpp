#ifndef __SERVER_RCON_RCON_HPP__
#define __SERVER_RCON_RCON_HPP__

namespace Server {
    class Server;
}

namespace Server { namespace Rcon {

    class EntityManager;
    class SessionManager;

    class Rcon :
        private boost::noncopyable
    {
    private:
        Server& _server;
        boost::asio::io_service _ioService;
        boost::asio::ip::tcp::acceptor _acceptor;
        boost::asio::ip::tcp::socket* _newRequest;
        boost::thread* _thread;
        SessionManager* _sessionManager;
        EntityManager* _entityManager;

    public:
        Rcon(Server& server);
        ~Rcon();
        void Start();
        void Stop();
        SessionManager& GetSessionManager() { return *this->_sessionManager; }
        EntityManager& GetEntityManager() { return *this->_entityManager; }
    private:
        void _Run();
        void _ConnectAccept();
        void _HandleAccept(boost::system::error_code const& e);
    };

}}

#endif
