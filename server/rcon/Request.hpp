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
        boost::asio::streambuf _buffer;
        // ordre de remplissage
        std::string _header;
        std::string _method;
        std::string _urlString;
        std::vector<std::string> _url; // url découpée sur /
        std::string _token;
        std::string _userAgent;
        std::string _body;
        std::map<std::string, std::string> _content; // données de formulaire (décodées)

    public:
        Request(Server& server, boost::asio::ip::tcp::socket* socket);
        ~Request();
    private:
        /*
         * Methodes déclarées dans l'ordre d'execution
         */
        // lecture
        void _ReadHttpHeader();
        void _HttpHeaderReceived(boost::system::error_code const& error, std::size_t size);
        void _ParseHeader();
        // optionel
        void _ReadHttpBody(std::size_t size);
        void _HttpBodyReceived(boost::system::error_code const& error, std::size_t size);

        void _ParseBody();
        bool _DecodeUrl(std::string const& in, std::string& out); // utilisé dans _ParseBody() pour lire le x-www-form-urlencoded
        void _Execute(); // lecture de l'url et choix de la methode à appeler

        // requetes exposées
        void _PostLogin(std::string const& login, std::string const& password); // POST /login
        void _GetRconSessions(); // GET /rcon_sessions
        void _GetEntities(Game::Map::Map const& map); // GET /map/<map>/entities
        void _GetEntityFile(std::string const& pluginIdentifier, std::string const& file); // GET /entity_file/<plugin>/<file>
        void _PostEntityFile(std::string const& pluginIdentifier, std::string const& file, std::string const& lua); // POST /entity_file/<plugin>/<file>
        void _PostExecute(Game::Map::Map const& map, std::string const& pluginIdentifier, std::string const& lua); // POST /map/<map>/execute/<plugin>

        // réponse
        void _JsonCallback(std::string json);
        void _JsonCallbackDispatched(std::string const& json);
        void _WriteHttpResponse(std::string const& status, std::string const& content = std::string());
        void _HttpResponseWritten(boost::system::error_code const& error, std::size_t size);
    };

}}

#endif
