#ifndef __CLIENT_MENU_MAINMENU_HPP__
#define __CLIENT_MENU_MAINMENU_HPP__

namespace Tools {
    namespace Gfx {
        class IRenderer;
        namespace Utils {
            class Rectangle;
        }
    }
    namespace Window {
        class ActionBinder;
    }
}
namespace Client {
    class Client;
    namespace Menu { namespace Widget {
        class Button;
        class DebugPanel;
    }}
}

namespace Client { namespace Menu {

    class Menu;

    class MainMenu
    {
    private:
        Client& _client;
        Menu& _menu;
        Tools::Gfx::IRenderer& _renderer;
        glm::detail::tmat4x4<float> _backRectMatrix;
        Tools::Gfx::Utils::Rectangle* _backRect;
        int _callbackId;
        bool _visible;
        Tools::Window::ActionBinder* _actionBinder;
        Widget::Button* _quitButton;
        Widget::Button* _backButton;
        Widget::DebugPanel* _debugPanel;

    public:
        MainMenu(Client& client, Menu& menu);
        ~MainMenu();
        void SetVisible(bool visible = true, bool warpMouse = true);
        bool IsVisible() const { return this->_visible; }
        void Update();
        void Render();
    private:
        void _Resize(glm::uvec2 const& size);
        void _QuitButton();
        void _BackButton();
        void _MenuBind();
    };

}}

#endif
