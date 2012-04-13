#include "server/rcon/Request.hpp"
#include "server/rcon/SessionManager.hpp"

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
            std::string s((std::istreambuf_iterator<char>(&this->_buffer)), std::istreambuf_iterator<char>());
            Tools::log << s << std::endl;
            delete this; // XXX
        }
        else
        {
            Tools::error << "Rcon: Error when reading HTTP header: " << error.message() << std::endl;
            delete this;
        }
    }

}}
