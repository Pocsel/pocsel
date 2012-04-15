#ifndef __SERVER_RCON_REQUEST_HPP__
#define __SERVER_RCON_REQUEST_HPP__

namespace Server {
    class Server;
    namespace Game { namespace Map {
            class Map;
    }}
}

namespace Server { namespace Rcon {

    class SessionManager;

    class Request :
        private boost::noncopyable
    {
    private:
        enum
        {
            MaxContentLength = 1048576, // 1 mo
        };

    private:
        Server& _server;
        boost::asio::ip::tcp::socket* _socket;
        SessionManager& _sessionManager;
        boost::asio::streambuf _buffer;
        // ordre de remplissage
        std::string _header;
        std::string _method;
        std::string _urlString;
        std::vector<std::string> _url;
        std::string _token;
        std::string _body;

    public:
        Request(Server& server, boost::asio::ip::tcp::socket* socket, SessionManager& sessionManager);
        ~Request();
    private:
        // ordre d'execution
        void _ReadHttpHeader();
        void _HttpHeaderReceived(boost::system::error_code const& error, std::size_t size);
        void _ParseHeader();
        void _ReadHttpBody(std::size_t size);
        void _HttpBodyReceived(boost::system::error_code const& error, std::size_t size);

        void _Execute();
        void _Login();
        void _GetEntities(Game::Map::Map const& map);

        void _JsonCallback(std::string json);
        void _JsonCallbackDispatched(std::string const& json);
        void _WriteHttpResponse(std::string const& status, std::string const& content = std::string());
        void _HttpResponseWritten(boost::system::error_code const& error, std::size_t size);
    };

}}

#endif
