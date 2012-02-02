#include <iostream>
#include <functional>

#include <boost/thread.hpp>

#include "common/Packet.hpp"

#include "resources/resources.hpp"

#include "tools/gui/Window.hpp"
#include "tools/gui/WidgetFactory.hpp"

#include "client/Application.hpp"
#include "client/InputBinder.hpp"
#include "client/Network.hpp"
#include "client/gui/GameWidget.hpp"


using namespace Client;

Application::Application(Tools::Gui::Window &mainWindow) :
    Tools::Gui::Application(mainWindow),
    _packetDispatcher(_eventManager),
    _network(0),
    _user(0),
    _game(0),
    _inputBinder(0),
    _window(mainWindow)
{
    this->_LoadDefaultSettings();
    this->_network = new Network(
        std::bind(&Application::_OnNetworkPacket, this, std::placeholders::_1),
        std::bind(&Application::_OnNetworkError, this, std::placeholders::_1)
    );
    this->_user = new User(*this);
    this->_game = new Game(*this);
    this->_mainWindow.GetViewport().AddChild(
        std::unique_ptr<Gui::GameWidget>(new Gui::GameWidget(this->GetEventManager(), *this->_game))
    );
    this->_inputBinder = new InputBinder(this->GetEventManager());
    this->_inputBinder->LoadFile(this->_settings["user.key_bindings_path"]);
    // XXX Mettre dans les settings
    Tools::Gui::Widget::defaultStylesheet().SetStyle("font-file", RESOURCES_DIR "/DejaVuSerif-Italic.ttf");
    Tools::Gui::Widget::defaultStylesheet().SetStyle("font-size", "16px");
}

Application::~Application()
{
    Tools::Delete(this->_inputBinder);
    Tools::Delete(this->_game);
    Tools::Delete(this->_user);
    Tools::Delete(this->_network);
}

int Application::Run(int ac, char *av[])
{
    int port = 8173;
    std::string ip = "127.0.0.1";

    if (ac > 1)
        port = atoi(av[1]);
    if (ac > 2)
        ip = av[2];

    try // TODO: virer ce try/catch
    {
        this->_network->Connect(ip, port);
        boost::thread networkThread(std::bind(&Network::Run, this->_network));
        this->_user->Login();
        this->_eventManager.Run();
        std::cout << "Application::Run(): EventManager stopped\n";
        this->_network->Stop();
        std::cout << "Application::Run(): Network stopped\n";
        networkThread.join();
        return 0;
    }
    catch (std::exception& ex)
    {
        std::cerr << "Applicatation::Run() Exception: " << ex.what() << std::endl;
        this->_eventManager.Run();
        return 1;
    }
}

void Application::_OnNetworkPacket(Common::PacketPtr& packet)
{
    this->_packetDispatcher.PushPacket(packet);
}

void Application::_OnNetworkError(std::string const& error)
{
    std::cerr << "Network error: " << error << std::endl;
    this->_eventManager.Stop();
}

void Application::_LoadDefaultSettings()
{
    this->_settings["user.login"] = "yalap_a";
    this->_settings["user.key_bindings_path"] = BINDS_PATH;
}
