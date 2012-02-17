#include "client/menu/Menu.hpp"
#include "client/Client.hpp"
#include "client/resources/LocalResourceManager.hpp"
#include "client/window/Window.hpp"
#include "tools/IRenderer.hpp"
#include "client/menu/LoadingScreen.hpp"
#include "client/menu/DisconnectedScreen.hpp"

namespace Client { namespace Menu {

    Menu::Menu(Client& client) :
        _client(client)
    {
        this->_font = &client.GetLocalResourceManager().GetFont("Acme_7_Wide.ttf", 10);
        this->_fontShader = &client.GetLocalResourceManager().GetShader("Fonts.cgfx");
        this->_fontColor = this->_fontShader->GetParameter("color").release();
        this->_fontTexture = this->_fontShader->GetParameter("fontTex").release();
        this->_rectShader = &client.GetLocalResourceManager().GetShader("BaseShaderColor.cgfx");
        this->_loadingScreen = new LoadingScreen(client, *this);
        this->_disconnectedScreen = new DisconnectedScreen(client, *this);
    }

    Menu::~Menu()
    {
        Tools::Delete(this->_disconnectedScreen);
        Tools::Delete(this->_loadingScreen);
        Tools::Delete(this->_fontTexture);
        Tools::Delete(this->_fontColor);
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
