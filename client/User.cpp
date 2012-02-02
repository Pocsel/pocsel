
#include <iostream>

#include "tools/gui/EventManager.hpp"
#include "common/Packet.hpp"

#include "User.hpp"
#include "Application.hpp"
#include "Network.hpp"
#include "PacketDispatcher.hpp"
#include "PacketCreator.hpp"
#include "PacketExtractor.hpp"
#include "CacheDatabaseProxy.hpp"

namespace Client
{

    User::User(Application& app) :
        _app(app),
        _login(app.GetSettings()["user.login"]),
        _loggedIn(false)
    {
        app.GetPacketDispatcher().Register(
            Protocol::ServerToClient::LoggedIn,
            std::bind(&User::_HandleLoggedIn, this, std::placeholders::_1)
        );
    }

    void User::Login()
    {
        auto p = PacketCreator::LoginPacket(this->_login);
        this->_app.GetNetwork().SendPacket(std::move(p));
    }

    void User::_HandleLoggedIn(Common::Packet& p)
    {
        bool status;
        Protocol::Version versionMajor, versionMinor;
        std::string reason, worldIdentifier, worldName;
        Uint32 worldVersion;
        Chunk::CubeType nbCubeTypes;
        PacketExtractor::ExtractLogin(p,
                status,
                versionMajor,
                versionMinor,
                reason,
                worldIdentifier,
                worldName,
                worldVersion,
                nbCubeTypes);

        if (versionMajor != Protocol::Version::Major ||
            versionMinor != Protocol::Version::Minor)
        {
            std::cerr << "User::_HandleLogin: Bad server version "
                      << Tools::ToString(versionMajor) << "."
                      << Tools::ToString(versionMinor) << "." << std::endl;
            this->_app.GetEventManager().Notify<Tools::Gui::Event>("user/login-fail");
            return;
        }
        this->_loggedIn = status;
        if (!this->_loggedIn)
        {
            std::cerr << "User::_HandleLogin: Login refused: \""
                      << reason << "\"." << std::endl;
            this->_app.GetEventManager().Notify<Tools::Gui::Event>("user/login-fail");
        }
        else
        {
            this->_app.GetEventManager().Notify<Tools::Gui::Event>("user/login-ok");
            std::cout << "User logged in" << std::endl;
            this->_app.GetGame().GetCacheDatabaseProxy().Load(
                this->_app.GetNetwork().GetHost(),
                worldIdentifier, worldVersion, worldName
            );
            this->_app.GetGame().GetCubeTypeManager().Load(nbCubeTypes);
        }
    }

    std::string const& User::GetLogin() const
    {
        return this->_login;
    }

    bool User::IsLoggedIn() const
    {
        return this->_loggedIn;
    }

}
