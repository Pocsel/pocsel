#include "client2/menu/LoadingScreen.hpp"
#include "tools/renderers/utils/Rectangle.hpp"
#include "tools/renderers/utils/Font.hpp"
#include "client2/Client.hpp"
#include "client2/window/Window.hpp"
#include "client2/menu/Menu.hpp"

namespace Client { namespace Menu {

    LoadingScreen::LoadingScreen(Client& client) :
        _client(client),
        _menu(client.GetMenu()),
        _renderer(client.GetWindow().GetRenderer())
    {
        this->_backRect = new Tools::Renderers::Utils::Rectangle(this->_renderer);
        this->_backRect->SetColor(
                Tools::Color4f(0.25, 0.25, 0.25, 1),
                Tools::Color4f(0.25, 0.25, 0.25, 1),
                Tools::Color4f(0.1, 0.1, 0.1, 1),
                Tools::Color4f(0.1, 0.1, 0.1, 1));
        this->_barRect = new Tools::Renderers::Utils::Rectangle(this->_renderer);
        this->_barRect->SetColor(
                Tools::Color4f(0.9, 0.1, 0.1, 1),
                Tools::Color4f(0.9, 0.1, 0.1, 1),
                Tools::Color4f(0.7, 0.1, 0.1, 1),
                Tools::Color4f(0.7, 0.1, 0.1, 1));
        this->_Resize(this->_client.GetWindow().GetSize());
    }

    LoadingScreen::~LoadingScreen()
    {
        Tools::Delete(this->_barRect);
        Tools::Delete(this->_backRect);
    }

    void LoadingScreen::_Resize(Tools::Vector2u const& size)
    {
        this->_text1Matrix = Tools::Matrix4<float>::CreateTranslation(10, size.h - 60, 0);
        this->_text2Matrix = Tools::Matrix4<float>::CreateTranslation(10, size.h - 40, 0);
    }

    void LoadingScreen::Render(std::string const& status, float progress)
    {
        this->_menu.BeginMenuDrawing();
        do
        {
            this->_menu.GetRectShader().BeginPass();
            this->_renderer.SetModelMatrix(Tools::Matrix4<float>::CreateScale(this->_client.GetWindow().GetSize().w, this->_client.GetWindow().GetSize().h, 1));
            this->_backRect->Render();
            this->_renderer.SetModelMatrix(Tools::Matrix4<float>::CreateScale(this->_client.GetWindow().GetSize().w * progress, 20, 1) *
                    Tools::Matrix4<float>::CreateTranslation(0, this->_client.GetWindow().GetSize().h - 20, 0));
            this->_barRect->Render();
        } while (this->_menu.GetRectShader().EndPass());
        do
        {
            this->_menu.GetFontShader().BeginPass();
            this->_renderer.SetModelMatrix(this->_text1Matrix);
            this->_menu.GetFont().Render(this->_menu.GetFontTexture(), "Loading...");
            this->_renderer.SetModelMatrix(this->_text2Matrix);
            this->_menu.GetFont().Render(this->_menu.GetFontTexture(), status);
        } while (this->_menu.GetFontShader().EndPass());
        this->_menu.EndMenuDrawing();
    }

}}
