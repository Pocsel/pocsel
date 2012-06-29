#include "client/precompiled.hpp"

#include "tools/renderers/utils/Rectangle.hpp"
#include "tools/window/Window.hpp"
#include "tools/window/ActionBinder.hpp"
#include "tools/window/InputManager.hpp"

#include "client/menu/MainMenu.hpp"
#include "client/Client.hpp"
#include "client/menu/Menu.hpp"
#include "client/menu/widget/Button.hpp"
#include "client/menu/widget/DebugPanel.hpp"

namespace Client { namespace Menu {

    MainMenu::MainMenu(Client& client, Menu& menu) :
        _client(client),
        _menu(menu),
        _renderer(client.GetWindow().GetRenderer()),
        _visible(false)
    {
        this->_backRect = new Tools::Renderers::Utils::Rectangle(this->_renderer);
        this->_backRect->SetColor(
                glm::vec4(0.25f, 0.25f, 0.25f, 0.7f),
                glm::vec4(0.25f, 0.25f, 0.25f, 0.7f),
                glm::vec4(0.25f, 0.25f, 0.25f, 0.7f),
                glm::vec4(0.25f, 0.25f, 0.25f, 0.7f));

        this->_actionBinder = new Tools::Window::ActionBinder();
        this->_actionBinder->Bind(Tools::Window::BindAction::Menu, Tools::Window::BindAction::Released, std::bind(&MainMenu::_MenuBind, this));
        this->_actionBinder->Bind(Tools::Window::BindAction::Quit, Tools::Window::BindAction::Released, std::bind(&Client::Quit, &this->_client));

        std::function<void(void)> f = std::bind(&MainMenu::_QuitButton, this);
        this->_quitButton = new Widget::Button(this->_client.GetWindow().GetInputManager(),
                this->_menu,
                this->_renderer,
                *this->_actionBinder,
                f,
                "Quit",
                glm::fvec2(200, 20));
        f = std::bind(&MainMenu::_BackButton, this);
        this->_backButton = new Widget::Button(this->_client.GetWindow().GetInputManager(),
                this->_menu,
                this->_renderer,
                *this->_actionBinder,
                f,
                "Back to game",
                glm::fvec2(200, 20));

        this->_callbackId = this->_client.GetWindow().RegisterCallback(std::bind(&MainMenu::_Resize, this, std::placeholders::_1));
        this->_Resize(this->_client.GetWindow().GetSize());

        this->_debugPanel = new Widget::DebugPanel(this->_menu, this->_renderer);
    }

    MainMenu::~MainMenu()
    {
        this->_client.GetWindow().UnregisterCallback(this->_callbackId);
        Tools::Delete(this->_debugPanel);
        Tools::Delete(this->_backRect);
        Tools::Delete(this->_backButton);
        Tools::Delete(this->_quitButton);
        Tools::Delete(this->_actionBinder);
    }

    void MainMenu::Update()
    {
        this->_actionBinder->Dispatch(this->_client.GetWindow().GetInputManager(), true);
        this->_client.GetWindow().GetInputManager().ShowMouse();
    }

    void MainMenu::Render()
    {
        this->_menu.BeginMenuDrawing();
        do
        {
            this->_menu.GetRectShader().BeginPass();
            this->_renderer.SetModelMatrix(this->_backRectMatrix);
            this->_backRect->Render();
        } while (this->_menu.GetRectShader().EndPass());
        this->_quitButton->Render();
        this->_backButton->Render();

        this->_debugPanel->Render();
        this->_menu.EndMenuDrawing();
    }

    void MainMenu::SetVisible(bool visible /* = true */, bool warpMouse /* = true */)
    {
        if (this->_visible != visible && warpMouse)
            this->_client.GetWindow().GetInputManager().WarpMouse(
                    this->_client.GetWindow().GetSize().x / 2,
                    this->_client.GetWindow().GetSize().y / 2);
        this->_visible = visible;
    }

    void MainMenu::_Resize(glm::uvec2 const& sz)
    {
        glm::fvec2 size(sz);
        this->_backRectMatrix =
            glm::translate<float>(size.x / 2, size.y / 2, 0)
            * glm::scale<float>(size.x / 2 + 1, size.y / 2 + 1, 1);
        this->_quitButton->SetPos(glm::fvec2(size.x / 2 - this->_quitButton->GetSize().x / 2, size.y - 80));
        this->_backButton->SetPos(glm::fvec2(size.x / 2 - this->_quitButton->GetSize().x / 2, size.y - 40));
    }

    void MainMenu::_QuitButton()
    {
        this->_client.GetWindow().GetInputManager().TriggerAction(Tools::Window::BindAction::Quit, Tools::Window::BindAction::Released);
    }

    void MainMenu::_BackButton()
    {
        this->SetVisible(false);
    }

    void MainMenu::_MenuBind()
    {
        this->SetVisible(false);
    }

}}
