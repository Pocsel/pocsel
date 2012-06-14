#include "tools/window/Window.hpp"
#include "tools/IRenderer.hpp"

#include "client/menu/Menu.hpp"
#include "client/Client.hpp"
#include "client/resources/LocalResourceManager.hpp"
#include "client/menu/LoadingScreen.hpp"
#include "client/menu/DisconnectedScreen.hpp"
#include "client/menu/MainMenu.hpp"

namespace Client { namespace Menu {

    Menu::Menu(Client& client) :
        _client(client)
    {
        this->_font = &client.GetLocalResourceManager().GetFont("Acme_7_Wide.ttf", 20);
        this->_fontShader = &client.GetLocalResourceManager().GetShader("Fonts.fx");
        this->_fontColor = &this->_fontShader->GetParameter("color");
        this->_fontTexture = &this->_fontShader->GetParameter("fontTex");
        this->_rectShader = &client.GetLocalResourceManager().GetShader("BaseShaderColor.fx");
        this->_loadingScreen = new LoadingScreen(client, *this);
        this->_disconnectedScreen = new DisconnectedScreen(client, *this);
        this->_mainMenu = new MainMenu(client, *this);
    }

    Menu::~Menu()
    {
        Tools::Delete(this->_mainMenu);
        Tools::Delete(this->_disconnectedScreen);
        Tools::Delete(this->_loadingScreen);
    }

    void Menu::BeginMenuDrawing()
    {
        this->_client.GetWindow().GetRenderer().BeginDraw2D();
        this->_fontColor->Set(Tools::Color4f(1, 1, 1, 1));
    }

    void Menu::EndMenuDrawing()
    {
        this->_client.GetWindow().GetRenderer().EndDraw2D();
    }

}}
