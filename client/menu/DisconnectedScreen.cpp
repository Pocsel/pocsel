#include "client/menu/DisconnectedScreen.hpp"
#include "tools/renderers/utils/Rectangle.hpp"
#include "tools/renderers/utils/Font.hpp"
#include "client/Client.hpp"
#include "client/window/Window.hpp"
#include "client/menu/Menu.hpp"

namespace Client { namespace Menu {

    DisconnectedScreen::DisconnectedScreen(Client& client, Menu& menu) :
        _client(client),
        _menu(menu),
        _renderer(client.GetWindow().GetRenderer())
    {
        this->_backRect = new Tools::Renderers::Utils::Rectangle(this->_renderer);
        this->_backRect->SetColor(
                Tools::Color4f(0.31f, 0.03f, 0.03f, 1),
                Tools::Color4f(0.31f, 0.03f, 0.03f, 1),
                Tools::Color4f(0.1f, 0.01f, 0.01f, 1),
                Tools::Color4f(0.1f, 0.01f, 0.01f, 1));
        this->_callbackId = this->_client.GetWindow().RegisterCallback(std::bind(&DisconnectedScreen::_Resize, this, std::placeholders::_1));
        this->_Resize(this->_client.GetWindow().GetSize());
    }

    DisconnectedScreen::~DisconnectedScreen()
    {
        this->_client.GetWindow().UnregisterCallback(this->_callbackId);
        Tools::Delete(this->_backRect);
    }

    void DisconnectedScreen::SetMessage(std::string const& message)
    {
        this->_message = message;
    }

    void DisconnectedScreen::_Resize(Tools::Vector2u const& size)
    {
        this->_text1Matrix = Tools::Matrix4<float>::CreateTranslation(10, size.h - 80, 0);
        this->_text2Matrix = Tools::Matrix4<float>::CreateTranslation(10, size.h - 60, 0);
        this->_backRectMatrix = Tools::Matrix4<float>::CreateScale(size.w / 2, size.h / 2, 1)
            * Tools::Matrix4<float>::CreateTranslation(size.w / 2, size.h / 2, 0);
    }

    void DisconnectedScreen::Render()
    {
        this->_menu.BeginMenuDrawing();
        do
        {
            this->_menu.GetRectShader().BeginPass();
            this->_renderer.SetModelMatrix(this->_backRectMatrix);
            this->_backRect->Render();
        } while (this->_menu.GetRectShader().EndPass());
        do
        {
            this->_menu.GetFontShader().BeginPass();
            this->_renderer.SetModelMatrix(this->_text1Matrix);
            this->_menu.GetFont().Render(this->_menu.GetFontTexture(), "Disconnected");
            this->_renderer.SetModelMatrix(this->_text2Matrix);
            this->_menu.GetFont().Render(this->_menu.GetFontTexture(), this->_message);
        } while (this->_menu.GetFontShader().EndPass());
        this->_menu.EndMenuDrawing();
    }

}}
