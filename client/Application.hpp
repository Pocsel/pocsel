#ifndef __CLIENT_APPLICATION_HPP__
#define __CLIENT_APPLICATION_HPP__

#include <string>

#include <boost/noncopyable.hpp>

#include "tools/gui/Application.hpp"
#include "common/PacketPtr.hpp"

#include "gui/GameWidget.hpp"
#include "Game.hpp"
#include "Network.hpp"
#include "PacketDispatcher.hpp"
#include "User.hpp"

namespace Client {

    class InputBinder;

    class Application : public Tools::Gui::Application
    {
    private:
        PacketDispatcher _packetDispatcher;
        Network* _network;
        User* _user;
        Game* _game;
        InputBinder* _inputBinder;
        Tools::Gui::Window& _window;

    public:
        Application(Tools::Gui::Window &mainWindow);
        virtual ~Application();
        virtual int Run(int ac, char *av[]);
        Network& GetNetwork() { return *this->_network; }
        PacketDispatcher& GetPacketDispatcher() { return this->_packetDispatcher; }
        User& GetUser() { return *this->_user; }
        Game& GetGame() { return *this->_game; }
        InputBinder& GetInputBinder() { return *this->_inputBinder; }
        Tools::Gui::Window& GetWindow() { return this->_window; }

    private:
        void _OnNetworkPacket(Common::PacketPtr& packet);
        void _OnNetworkError(std::string const& error);
        void _LoadDefaultSettings();
    };

}

#endif
