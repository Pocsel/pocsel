#include "client/precompiled.hpp"

#include "tools/renderers/utils/Rectangle.hpp"
#include "tools/renderers/utils/Font.hpp"
#include "tools/window/ActionBinder.hpp"
#include "tools/window/InputManager.hpp"
#include "tools/window/Window.hpp"

#include "client/menu/widget/Button.hpp"
#include "client/menu/DisconnectedScreen.hpp"
#include "client/menu/Menu.hpp"
#include "client/Client.hpp"

namespace Client { namespace Menu {

    DisconnectedScreen::DisconnectedScreen(Client& client, Menu& menu) :
        _client(client),
        _menu(menu),
        _renderer(client.GetWindow().GetRenderer())
    {
        this->_actionBinder = new Tools::Window::ActionBinder();
        std::function<void(void)> f = std::bind(&DisconnectedScreen::_RetryButton, this);
        this->_button = new Widget::Button(this->_client.GetWindow().GetInputManager(),
                this->_menu,
                this->_renderer,
                *this->_actionBinder,
                f,
                "Retry",
                glm::fvec2(100, 20));
        this->_backRect = new Tools::Renderers::Utils::Rectangle(this->_renderer);
        this->_backRect->SetColor(
                glm::vec4(0.31f, 0.03f, 0.03f, 1),
                glm::vec4(0.31f, 0.03f, 0.03f, 1),
                glm::vec4(0.1f, 0.01f, 0.01f, 1),
                glm::vec4(0.1f, 0.01f, 0.01f, 1));
        this->_callbackId = this->_client.GetWindow().RegisterCallback(std::bind(&DisconnectedScreen::_Resize, this, std::placeholders::_1));
        this->_Resize(this->_client.GetWindow().GetSize());
    }

    DisconnectedScreen::~DisconnectedScreen()
    {
        this->_client.GetWindow().UnregisterCallback(this->_callbackId);
        Tools::Delete(this->_backRect);
        Tools::Delete(this->_button);
        Tools::Delete(this->_actionBinder);
    }

    void DisconnectedScreen::_RetryButton()
    {
        this->_client.Connect();
    }

    void DisconnectedScreen::SetMessage(std::string const& message)
    {
        this->_message = message;
    }

    void DisconnectedScreen::_Resize(glm::uvec2 const& sz)
    {
        glm::fvec2 size(sz);
        this->_text1Matrix = glm::translate<float>(10, size.y - 80, 0) * glm::scale<float>(glm::fvec3(0.5f));
        this->_text2Matrix = glm::translate<float>(10, size.y - 60, 0) * glm::scale<float>(glm::fvec3(0.5f));
        this->_backRectMatrix =
            glm::translate<float>(size.x / 2, size.y / 2, 0)
            * glm::scale<float>(size.x / 2 + 1, size.y / 2 + 1, 1);
        this->_button->SetPos(glm::fvec2(10, size.y - 30));
    }

    void DisconnectedScreen::Render()
    {
        this->_client.GetWindow().GetInputManager().ShowMouse();
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
        this->_button->Render();
        this->_menu.EndMenuDrawing();
        this->_actionBinder->Dispatch(this->_client.GetWindow().GetInputManager());
    }

}}
