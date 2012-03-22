#include "client/menu/MainMenu.hpp"
#include "tools/renderers/utils/Rectangle.hpp"
#include "client/Client.hpp"
#include "client/window/Window.hpp"
#include "client/menu/Menu.hpp"
#include "client/menu/widget/Button.hpp"
#include "client/ActionBinder.hpp"
#include "client/window/InputManager.hpp"

namespace Client { namespace Menu {

    MainMenu::MainMenu(Client& client, Menu& menu) :
        _client(client),
        _menu(menu),
        _renderer(client.GetWindow().GetRenderer()),
        _visible(false)
    {
        this->_backRect = new Tools::Renderers::Utils::Rectangle(this->_renderer);
        this->_backRect->SetColor(
                Tools::Color4f(0.25f, 0.25f, 0.25f, 0.7f),
                Tools::Color4f(0.25f, 0.25f, 0.25f, 0.7f),
                Tools::Color4f(0.25f, 0.25f, 0.25f, 0.7f),
                Tools::Color4f(0.25f, 0.25f, 0.25f, 0.7f));

        this->_actionBinder = new ActionBinder();
        this->_actionBinder->Bind(BindAction::Menu, BindAction::Released, std::bind(&MainMenu::_MenuBind, this));
        this->_actionBinder->Bind(BindAction::Quit, BindAction::Released, std::bind(&Client::Quit, &this->_client));

        std::function<void(void)> f = std::bind(&MainMenu::_QuitButton, this);
        this->_quitButton = new Widget::Button(this->_client.GetWindow().GetInputManager(),
                this->_menu,
                this->_renderer,
                *this->_actionBinder,
                f,
                "Quit",
                Tools::Vector2f(200, 20));
        f = std::bind(&MainMenu::_BackButton, this);
        this->_backButton = new Widget::Button(this->_client.GetWindow().GetInputManager(),
                this->_menu,
                this->_renderer,
                *this->_actionBinder,
                f,
                "Back to game",
                Tools::Vector2f(200, 20));

        this->_callbackId = this->_client.GetWindow().RegisterCallback(std::bind(&MainMenu::_Resize, this, std::placeholders::_1));
        this->_Resize(this->_client.GetWindow().GetSize());
    }

    MainMenu::~MainMenu()
    {
        this->_client.GetWindow().UnregisterCallback(this->_callbackId);
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
        this->_menu.EndMenuDrawing();
    }

    void MainMenu::SetVisible(bool visible /* = true */, bool warpMouse /* = true */)
    {
        if (this->_visible != visible && warpMouse)
            this->_client.GetWindow().GetInputManager().WarpMouse(
                    this->_client.GetWindow().GetSize().w / 2,
                    this->_client.GetWindow().GetSize().h / 2);
        this->_visible = visible;
    }

    void MainMenu::_Resize(Tools::Vector2u const& sz)
    {
        Tools::Vector2f size(sz);
        this->_backRectMatrix = Tools::Matrix4<float>::CreateScale(size.w / 2 + 1, size.h / 2 + 1, 1)
            * Tools::Matrix4<float>::CreateTranslation(size.w / 2, size.h / 2, 0);
        this->_quitButton->SetPos(Tools::Vector2f(size.w / 2 - this->_quitButton->GetSize().w / 2, size.h - 80));
        this->_backButton->SetPos(Tools::Vector2f(size.w / 2 - this->_quitButton->GetSize().w / 2, size.h - 40));
    }

    void MainMenu::_QuitButton()
    {
        this->_client.GetWindow().GetInputManager().TriggerAction(BindAction::Quit, BindAction::Released);
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
