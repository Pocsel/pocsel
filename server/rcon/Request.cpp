#include <boost/tokenizer.hpp>

#include "server/rcon/Request.hpp"
#include "server/rcon/SessionManager.hpp"
#include "server/Server.hpp"
#include "server/game/Game.hpp"
#include "server/game/World.hpp"
#include "server/game/map/Map.hpp"
#include "server/ProgramInfo.hpp"

namespace Server { namespace Rcon {

    Request::Request(Server& server, boost::asio::ip::tcp::socket* socket, SessionManager& sessionManager) :
        _server(server),
        _socket(socket),
        _sessionManager(sessionManager)
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
            this->_header.assign((std::istreambuf_iterator<char>(&this->_buffer)), std::istreambuf_iterator<char>());
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
        // method
        auto it = this->_header.begin();
        auto itEnd = this->_header.end();
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

        // execution de la requete
        if (contentLength > MaxContentLength)
        {
            Tools::error << "Rcon: Content-Length of " << contentLength << " bytes too big (max: " << MaxContentLength << ")." << std::endl;
            this->_WriteHttpResponse("413 Request Entity Too Large");
        }
        else if (contentLength)
            this->_ReadHttpBody(contentLength);
        else
            this->_Execute();
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
            this->_body.assign((std::istreambuf_iterator<char>(&this->_buffer)), std::istreambuf_iterator<char>());
            this->_Execute();
        }
        else
        {
            Tools::error << "Rcon: Error while reading HTTP body: " << error.message() << std::endl;
            delete this;
        }
    }

    void Request::_Execute()
    {
        if (!this->_url.empty())
        {
            if (this->_url[0] == "map" && this->_url.size() == 3)
            {
                if (this->_server.GetGame().GetWorld().HasMap(this->_url[1]))
                {
                    Game::Map::Map const& map = this->_server.GetGame().GetWorld().GetMap(this->_url[1]);
                    if (this->_url[2] == "entities" && this->_method == "GET")
                        return this->_GetEntities(map);
                }
            }
            else if (this->_url[0] == "login" && this->_method == "POST")
                return this->_Login();
        }
        this->_WriteHttpResponse("404 Not Found");
    }

    void Request::_Login()
    {
        // TODO lire login/pass dans le POST et les utiliser
        std::string newToken = this->_sessionManager.NewSession("login", "pass");
        if (newToken.empty())
            return this->_WriteHttpResponse("401 Unauthorized");
        std::string json = "{\n"
            "\t\"token\": \"" + newToken + "\",\n"
            "\t\"maps\":\n"
            "\t[\n";
        auto const& maps = this->_server.GetGame().GetWorld().GetMaps();
        auto it = maps.begin();
        auto itEnd = maps.end();
        for (; it != itEnd; ++it)
        {
            if (it != maps.begin())
                json += ",\n";
            json += "\t\t{\n"
                "\t\t\t\"identifier\": \"" + it->second->GetName() + "\",\n"
                "\t\t\t\"fullname\": \"" + it->second->GetFullName() + "\"\n"
                "\t\t}";
        }
        json += "\n\t]\n"
            "}\n";
        this->_WriteHttpResponse("200 OK", json);
    }

    void Request::_GetEntities(Game::Map::Map const& map)
    {
        if (this->_sessionManager.HasRights(this->_token, "entities"))
            map.RconGetEntities(std::bind(&Request::_JsonCallback, this, std::placeholders::_1));
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
                "Server: " PROJECT_NAME " " PROGRAM_NAME " " GIT_VERSION "\r\n"
                "Content-Length: " + boost::lexical_cast<std::string>(content.size()) + "\r\n"
                "Connection: close\r\n"
                "Content-Type: application/json; charset=utf-8\r\n"
                "Cache-Control: no-cache, no-store, max-age=0, must-revalidate\r\n"
                "Pragma: no-cache\r\n"
                "\r\n");
        response += content;
        boost::asio::async_write(*this->_socket, boost::asio::buffer(response),
                boost::bind(&Request::_HttpResponseWritten, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
    }

    void Request::_HttpResponseWritten(boost::system::error_code const& error, std::size_t)
    {
        if (error)
            Tools::error << "Rcon: Error while writing HTTP response: " << error.message() << std::endl;
        delete this;
    }

}}
