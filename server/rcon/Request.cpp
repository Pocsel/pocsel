#include <boost/tokenizer.hpp>

#include "server/rcon/Request.hpp"
#include "server/rcon/Rcon.hpp"
#include "server/rcon/SessionManager.hpp"
#include "server/rcon/EntityManager.hpp"
#include "server/rcon/ToJsonStr.hpp"
#include "server/Server.hpp"
#include "server/Settings.hpp"
#include "server/game/Game.hpp"
#include "server/game/World.hpp"
#include "server/game/map/Map.hpp"
#include "server/game/PluginManager.hpp"
#include "server/ProgramInfo.hpp"

namespace Server { namespace Rcon {

    Request::Request(Server& server, boost::asio::ip::tcp::socket* socket) :
        _server(server),
        _socket(socket)
    {
        Tools::debug << "Rcon: New request." << std::endl;
        this->_ReadHttpHeader();
    }

    Request::~Request()
    {
        Tools::debug << "Rcon: Request finished." << std::endl;
        Tools::Delete(this->_socket);
    }

    void Request::_ReadHttpHeader()
    {
        boost::asio::async_read_until(*this->_socket, this->_buffer, "\r\n\r\n",
                boost::bind(&Request::_HttpHeaderReceived, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
    }

    void Request::_HttpHeaderReceived(boost::system::error_code const& error, std::size_t)
    {
        if (!error)
        {
            std::string buf((std::istreambuf_iterator<char>(&this->_buffer)), std::istreambuf_iterator<char>());
            std::size_t bufferEnd = buf.find("\r\n\r\n");
            assert(bufferEnd != std::string::npos && "bug report boost async_read_until needed");
            this->_header = buf.substr(0, bufferEnd + 4);
            this->_body = buf.substr(bufferEnd + 4);
            this->_ParseHeader();
        }
        else
        {
            Tools::error << "Rcon: Error while reading HTTP header: " << error.message() << std::endl;
            delete this;
        }
    }

    void Request::_ParseHeader()
    {
        auto it = this->_header.begin();
        auto itEnd = this->_header.end();

        // method
        while (it != itEnd && *it != ' ')
        {
            this->_method += *it;
            ++it;
        }

        // url
        if (it != itEnd)
        {
            ++it;
            while (it != itEnd && *it != ' ')
            {
                this->_urlString += *it;
                ++it;
            }
        }
        boost::char_separator<char> sep("/");
        boost::tokenizer<boost::char_separator<char>> tokens(this->_urlString, sep);
        auto tokensIt = tokens.begin();
        auto tokensItEnd = tokens.end();
        for (; tokensIt != tokensItEnd; ++tokensIt)
            if (!tokensIt->empty())
                this->_url.push_back(*tokensIt);

        // content-length
        std::size_t contentLength = 0;
        std::size_t contentLengthPos = this->_header.find("\nContent-Length: "); // 17 chars
        if (contentLengthPos != std::string::npos && contentLengthPos + 18 < this->_header.size())
        {
            std::string tmpStr;
            it = this->_header.begin() + contentLengthPos + 17;
            while (it != itEnd && *it != '\r')
            {
                tmpStr += *it;
                ++it;
            }
            contentLength = boost::lexical_cast<std::size_t>(tmpStr);
        }

        // token
        std::size_t tokenPos = this->_header.find("\nRcon-Token: "); // 13 chars
        if (tokenPos != std::string::npos && tokenPos + 14 < this->_header.size())
        {
            it = this->_header.begin() + tokenPos + 13;
            while (it != itEnd && *it != '\r')
            {
                this->_token += *it;
                ++it;
            }
        }

        // user agent
        std::size_t userAgentPos = this->_header.find("\nUser-Agent: "); // 13 chars
        if (userAgentPos != std::string::npos && userAgentPos + 14 < this->_header.size())
        {
            it = this->_header.begin() + userAgentPos + 13;
            while (it != itEnd && *it != '\r')
            {
                this->_userAgent += *it;
                ++it;
            }
        }

        // execution de la requete
        if (contentLength > MaxContentLength || this->_body.size() > contentLength)
        {
            Tools::error << "Rcon: Content-Length of " << contentLength << " bytes too big (max: " << MaxContentLength << ")." << std::endl;
            this->_WriteHttpResponse("413 Request Entity Too Large");
        }
        else if (contentLength && this->_body.size() < contentLength)
            this->_ReadHttpBody(contentLength - this->_body.size());
        else
            this->_ParseBody();
    }

    void Request::_ReadHttpBody(std::size_t size)
    {
        boost::asio::async_read(*this->_socket, this->_buffer, boost::asio::transfer_at_least(size),
                boost::bind(&Request::_HttpBodyReceived, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
    }

    void Request::_HttpBodyReceived(boost::system::error_code const& error, std::size_t)
    {
        if (!error)
        {
            std::string buf((std::istreambuf_iterator<char>(&this->_buffer)), std::istreambuf_iterator<char>());
            this->_body += buf;
            this->_ParseBody();
        }
        else
        {
            Tools::error << "Rcon: Error while reading HTTP body: " << error.message() << std::endl;
            delete this;
        }
    }

    void Request::_ParseBody()
    {
        bool inKey = true;
        std::string curKey;
        std::string curValue;
        std::string decodedKey;
        std::string decodedValue;
        auto it = this->_body.begin();
        auto itEnd = this->_body.end();
        for (; it != itEnd; ++it)
            switch (*it)
            {
                case '&':
                    inKey = true;
                    if (this->_DecodeUrl(curKey, decodedKey) && this->_DecodeUrl(curValue, decodedValue))
                        this->_content[decodedKey] = decodedValue;
                    curKey.clear();
                    curValue.clear();
                    break;
                case '=':
                    inKey = false;
                    break;
                default:
                    if (inKey)
                        curKey += *it;
                    else
                        curValue += *it;
            }
        if (!curKey.empty() && this->_DecodeUrl(curKey, decodedKey) && this->_DecodeUrl(curValue, decodedValue))
            this->_content[decodedKey] = decodedValue;

        this->_Execute();
    }

    void Request::_Execute()
    {
        Tools::debug << "Rcon: Header: \"" << this->_header << "\"" << std::endl;
        Tools::debug << "Rcon: Body: \"" << this->_body << "\"" << std::endl;
        if (this->_method == "OPTIONS")
            return this->_WriteHttpResponse("200 OK"); // pre-flight "allow cross-domain" ajax queries
        else if (!this->_url.empty())
        {
            if (this->_url[0] == "map" && this->_url.size() >= 3)
            {
                if (this->_server.GetGame().GetWorld().HasMap(this->_url[1]))
                {
                    Game::Map::Map const& map = this->_server.GetGame().GetWorld().GetMap(this->_url[1]);
                    if (this->_url[2] == "entities" && this->_url.size() == 3 && this->_method == "GET")
                        return this->_GetEntities(map); // GET /map/<map>/entities
                    else if (this->_url[2] == "execute" && this->_url.size() == 4 && this->_method == "POST")
                        return this->_PostExecute(map, this->_url[3], this->_content["lua"]); // POST /map/<map>/execute/<plugin>
                }
            }
            else if (this->_url[0] == "login" && this->_method == "POST")
                return this->_PostLogin(this->_content["login"], this->_content["password"]); // POST /login
            else if (this->_url[0] == "rcon_sessions" && this->_method == "GET")
                return this->_GetRconSessions(); // GET /rcon_sessions
            else if (this->_url[0] == "entity_file" && this->_url.size() == 3 && this->_method == "GET")
                return this->_GetEntityFile(this->_url[1], this->_url[2]); // GET /entity_file/<plugin>/<file>
            else if (this->_url[0] == "entity_file" && this->_url.size() == 3 && this->_method == "POST")
                return this->_PostEntityFile(this->_url[1], this->_url[2], this->_content["lua"]); // POST /entity_file/<plugin>/<file>
        }
        this->_WriteHttpResponse("404 Not Found");
    }

    void Request::_PostLogin(std::string const& login, std::string const& password)
    {
        // credentials checks
        std::list<std::string> rights;
        std::string newToken = this->_server.GetRcon().GetSessionManager().NewSession(login, password, this->_userAgent, rights);
        if (newToken.empty())
            return this->_WriteHttpResponse("401 Unauthorized");

        // rights
        std::string json = "{\n"
            "\t\"token\": \"" + newToken + "\",\n"
            "\t\"rights\":\n"
            "\t[\n";
        auto it = rights.begin();
        auto itEnd = rights.end();
        for (; it != itEnd; ++it)
        {
            if (it != rights.begin())
                json += ",\n";
            json += "\t\t\"" + ToJsonStr(*it) + "\"";
        }
        json += "\n\t],\n";

        // world
        json += "\t\"world_identifier\": \"" + this->_server.GetGame().GetWorld().GetIdentifier() + "\",\n"
            "\t\"world_fullname\": \"" + ToJsonStr(this->_server.GetGame().GetWorld().GetFullname()) + "\",\n"
            "\t\"world_version\": " + Tools::ToString(this->_server.GetGame().GetWorld().GetVersion()) + ",\n"
            "\t\"debug\": " + (this->_server.GetSettings().debug ? "true" : "false") + ",\n";

        // maps
        json += "\t\"maps\":\n";
        json += this->_server.GetGame().GetWorld().RconGetMaps() + ",\n";

        // plugins
        json += "\t\"plugins\":\n";
        json += this->_server.GetGame().GetWorld().GetPluginManager().RconGetPlugins() + ",\n";

        // entity types
        json += "\t\"entity_types\":\n";
        json += this->_server.GetRcon().GetEntityManager().RconGetEntityTypes() + ",\n";

        // entity files
        json += "\t\"entity_files\":\n";
        json += this->_server.GetRcon().GetEntityManager().RconGetEntityFiles();

        // send response
        json += "}\n";
        this->_WriteHttpResponse("200 OK", json);
    }

    void Request::_GetRconSessions()
    {
        if (this->_server.GetRcon().GetSessionManager().HasRights(this->_token, "rcon_sessions"))
            this->_WriteHttpResponse("200 OK", this->_server.GetRcon().GetSessionManager().RconGetSessions());
        else
            this->_WriteHttpResponse("401 Unauthorized");
    }

    void Request::_GetEntities(Game::Map::Map const& map)
    {
        if (this->_server.GetRcon().GetSessionManager().HasRights(this->_token, "entities"))
            map.RconGetEntities(std::bind(&Request::_JsonCallback, this, std::placeholders::_1));
        else
            this->_WriteHttpResponse("401 Unauthorized");
    }

    void Request::_GetEntityFile(std::string const& pluginIdentifier, std::string const& file)
    {
        if (this->_server.GetRcon().GetSessionManager().HasRights(this->_token, "execute"))
        {
            std::string json = this->_server.GetRcon().GetEntityManager().GetFile(pluginIdentifier, file);
            if (json.empty())
                this->_WriteHttpResponse("404 Not Found");
            else
                this->_WriteHttpResponse("200 OK", json);
        }
        else
            this->_WriteHttpResponse("401 Unauthorized");
    }

    void Request::_PostEntityFile(std::string const& pluginIdentifier, std::string const& file, std::string const& lua)
    {
        if (this->_server.GetRcon().GetSessionManager().HasRights(this->_token, "execute"))
        {
            if (this->_server.GetRcon().GetEntityManager().UpdateFile(pluginIdentifier, file, lua))
                this->_WriteHttpResponse("200 OK");
            else
                this->_WriteHttpResponse("404 Not Found");
        }
        else
            this->_WriteHttpResponse("401 Unauthorized");
    }

    void Request::_PostExecute(Game::Map::Map const& map, std::string const& pluginIdentifier, std::string const& lua)
    {
        if (this->_server.GetRcon().GetSessionManager().HasRights(this->_token, "execute"))
        {
            Uint32 pluginId = this->_server.GetGame().GetWorld().GetPluginManager().GetPluginId(pluginIdentifier);
            if (pluginId)
                map.RconExecute(pluginId, lua, std::bind(&Request::_JsonCallback, this, std::placeholders::_1));
            else
                this->_WriteHttpResponse("404 Not Found");
        }
        else
            this->_WriteHttpResponse("401 Unauthorized");
    }

    void Request::_JsonCallback(std::string json)
    {
        this->_socket->get_io_service().dispatch(std::bind(&Request::_JsonCallbackDispatched, this, json));
    }

    void Request::_JsonCallbackDispatched(std::string const& json)
    {
        this->_WriteHttpResponse("200 OK", json);
    }

    void Request::_WriteHttpResponse(std::string const& status, std::string const& content /* = std::string() */)
    {
        Tools::log << "Rcon: Request for \"" << this->_urlString << "\" (" << this->_method << "): " << status << "." << std::endl;
        std::string response(
                "HTTP/1.1 " + status + "\r\n"
                "Access-Control-Allow-Origin: *\r\n"
                "Access-Control-Allow-Methods: POST, GET, OPTIONS\r\n"
                "Access-Control-Max-Age: 3600\r\n"
                "Access-Control-Allow-Headers: Rcon-Token\r\n"
                "Server: " PROJECT_NAME " " PROGRAM_NAME " " GIT_VERSION "\r\n"
                "Content-Length: " + Tools::ToString(content.size()) + "\r\n"
                "Connection: close\r\n"
                "Content-Type: application/json; charset=utf-8\r\n"
                "Cache-Control: no-cache, no-store, max-age=0, must-revalidate\r\n"
                "Pragma: no-cache\r\n"
                "\r\n");
        response += content;
        Tools::debug << "Rcon: Response: \"" << response << "\"" << std::endl;
        boost::asio::async_write(*this->_socket, boost::asio::buffer(response),
                boost::bind(&Request::_HttpResponseWritten, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
    }

    void Request::_HttpResponseWritten(boost::system::error_code const& error, std::size_t)
    {
        if (error)
            Tools::error << "Rcon: Error while writing HTTP response: " << error.message() << std::endl;
        delete this;
    }

    bool Request::_DecodeUrl(std::string const& in, std::string& out)
    {
        out.clear();
        out.reserve(in.size());
        for (std::size_t i = 0; i < in.size(); ++i)
            if (in[i] == '%')
            {
                if (i + 3 <= in.size())
                {
                    int value = 0;
                    std::istringstream is(in.substr(i + 1, 2));
                    if (is >> std::hex >> value)
                    {
                        out += static_cast<char>(value);
                        i += 2;
                    }
                    else
                        return false;
                }
                else
                    return false;
            }
            else if (in[i] == '+')
                out += ' ';
            else
                out += in[i];
        return true;
    }

}}
