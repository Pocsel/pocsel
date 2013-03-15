#include "tools/gfx/utils/Rectangle.hpp"
#include "tools/gfx/utils/Font.hpp"
#include "tools/window/Window.hpp"

#include "client/menu/LoadingScreen.hpp"
#include "client/Client.hpp"
#include "client/menu/Menu.hpp"

namespace Client { namespace Menu {

    LoadingScreen::LoadingScreen(Client& client, Menu& menu) :
        _client(client),
        _menu(menu),
        _renderer(client.GetWindow().GetRenderer())
    {
        this->_backRect = new Tools::Gfx::Utils::Rectangle(this->_renderer);
        this->_backRect->SetColor(
                glm::vec4(0.25f, 0.25f, 0.25f, 1),
                glm::vec4(0.25f, 0.25f, 0.25f, 1),
                glm::vec4(0.1f, 0.1f, 0.1f, 1),
                glm::vec4(0.1f, 0.1f, 0.1f, 1));
        this->_barRect = new Tools::Gfx::Utils::Rectangle(this->_renderer);
        this->_barRect->SetColor(
                glm::vec4(0.93f, 0.1f, 0.1f, 1),
                glm::vec4(0.93f, 0.1f, 0.1f, 1),
                glm::vec4(0.43f, 0.1f, 0.1f, 1),
                glm::vec4(0.43f, 0.1f, 0.1f, 1));
        this->_callbackId = this->_client.GetWindow().RegisterCallback(std::bind(&LoadingScreen::_Resize, this, std::placeholders::_1));
        this->_Resize(this->_client.GetWindow().GetSize());
    }

    LoadingScreen::~LoadingScreen()
    {
        this->_client.GetWindow().UnregisterCallback(this->_callbackId);
        Tools::Delete(this->_barRect);
        Tools::Delete(this->_backRect);
    }

    void LoadingScreen::_Resize(glm::uvec2 const& sz)
    {
        glm::fvec2 size(sz);
        this->_text1Matrix = glm::translate<float>(10.0f, size.y - 80.0f, 0.0f) * glm::scale<float>(glm::fvec3(0.5f));
        this->_text2Matrix = glm::translate<float>(10.0f, size.y - 60.0f, 0.0f) * glm::scale<float>(glm::fvec3(0.5f));
        this->_backRectMatrix =
            glm::translate<float>(size.x / 2, size.y / 2, 0)
            * glm::scale<float>(size.x / 2 + 1, size.y / 2 + 1, 1);
    }

    void LoadingScreen::Render(std::string const& status, float progress)
    {
        this->_menu.BeginMenuDrawing();
        do
        {
            this->_menu.GetRectShader().BeginPass();
            this->_renderer.SetModelMatrix(this->_backRectMatrix);
            this->_backRect->Render();
            this->_renderer.SetModelMatrix(
                    glm::translate<float>((float)(this->_client.GetWindow().GetSize().x / 2) * progress, (float)this->_client.GetWindow().GetSize().y - 10, 0)
                    * glm::scale<float>((float)(this->_client.GetWindow().GetSize().x / 2) * progress, 10, 1)
                    );
            this->_barRect->Render();
        } while (this->_menu.GetRectShader().EndPass());

        do
        {
            this->_menu.GetFontShader().BeginPass();
            this->_renderer.SetModelMatrix(this->_text1Matrix);
            this->_menu.GetFont().Render(this->_menu.GetFontTexture(), "Loading");
            this->_renderer.SetModelMatrix(this->_text2Matrix);
            this->_menu.GetFont().Render(this->_menu.GetFontTexture(), status);
        } while (this->_menu.GetFontShader().EndPass());
        this->_menu.EndMenuDrawing();
    }

}}
