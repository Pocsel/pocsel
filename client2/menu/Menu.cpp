#include "client2/menu/Menu.hpp"
#include "client2/Client.hpp"
#include "client2/resources/LocalResourceManager.hpp"
#include "client2/menu/LoadingScreen.hpp"
#include "client2/window/Window.hpp"
#include "tools/IRenderer.hpp"

namespace Client { namespace Menu {

    Menu::Menu(Client& client) :
        _client(client)
    {
        this->_font = &client.GetLocalResourceManager().GetFont("Acme_7_Wide.ttf", 10);
        this->_fontShader = &client.GetLocalResourceManager().GetShader("Fonts.cgfx");
        this->_fontColor = this->_fontShader->GetParameter("color").release();
        this->_fontTexture = this->_fontShader->GetParameter("fontTex").release();
        this->_rectShader = &client.GetLocalResourceManager().GetShader("BaseShaderColor.cgfx");
        this->_loadingScreen = new LoadingScreen(client);
    }

    Menu::~Menu()
    {
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
