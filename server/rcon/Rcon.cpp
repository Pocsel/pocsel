#include "server/rcon/Rcon.hpp"
#include "server/rcon/Request.hpp"
#include "server/rcon/SessionManager.hpp"
#include "server/rcon/EntityManager.hpp"
#include "server/Server.hpp"
#include "server/Settings.hpp"

namespace Server { namespace Rcon {

    Rcon::Rcon(Server& server) :
        _server(server),
        _acceptor(this->_ioService),
        _thread(0)
    {
        Tools::debug << "Rcon::Rcon()" << std::endl;
        Settings const& settings = this->_server.GetSettings();
        this->_sessionManager = new SessionManager(settings);
        this->_entityManager = new EntityManager(this->_server);
        try
        {
            boost::asio::ip::tcp::resolver resolver(this->_ioService);
            boost::asio::ip::tcp::resolver::query query(
                    settings.rconHost,
                    settings.rconPort,
                    boost::asio::ip::tcp::resolver::query::passive
                    );
            boost::asio::ip::tcp::endpoint endpoint = *(resolver.resolve(query)); // take first
            this->_acceptor.open(endpoint.protocol());
            this->_acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
            this->_acceptor.bind(endpoint);
            this->_acceptor.listen();
            Tools::log <<
                "Rcon: Listening on " << endpoint.address().to_string()
                << ":" << settings.rconPort << std::endl;
        }
        catch (std::exception& e)
        {
            Tools::error <<
                "Rcon: Cannot bind to '" << settings.rconHost <<
                ":" << settings.rconPort <<
                "': " << e.what() << std::endl;
            throw;
        }
    }

    Rcon::~Rcon()
    {
        Tools::debug << "Rcon::~Rcon()" << std::endl;
        this->Stop();
        Tools::Delete(this->_entityManager);
        Tools::Delete(this->_sessionManager);
    }

    void Rcon::Start()
    {
        if (this->_thread)
            throw std::runtime_error("Rcon: Start() called while thread running");
        Tools::debug << "Rcon::Start()" << std::endl;
        this->_thread = new boost::thread(std::bind(&Rcon::_Run, this));
    }

    void Rcon::Stop()
    {
        if (!this->_thread)
            return;
        Tools::debug << "Rcon::Stop()" << std::endl;
        this->_ioService.stop();
        this->_thread->join();
        delete this->_thread;
        this->_thread = 0;
        Tools::Delete(this->_newRequest);
    }

    void Rcon::_Run()
    {
        this->_ConnectAccept();
        this->_ioService.run();
    }

    void Rcon::_ConnectAccept()
    {
        this->_newRequest = new boost::asio::ip::tcp::socket(this->_ioService);
        this->_acceptor.async_accept(
            *this->_newRequest,
            boost::bind(&Rcon::_HandleAccept, this, boost::asio::placeholders::error)
        );
    }

    void Rcon::_HandleAccept(boost::system::error_code const& e)
    {
        if (!e)
        {
            new Request(this->_server, this->_newRequest);
        }
        else
        {
            Tools::error << "Rcon: Error for new request: " << e.message() << std::endl;
            Tools::Delete(this->_newRequest);
        }
        this->_ConnectAccept();
    }

}}
