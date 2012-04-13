#ifndef __CLIENT_MENU_MAINMENU_HPP__
#define __CLIENT_MENU_MAINMENU_HPP__

#include "tools/Matrix4.hpp"

namespace Tools { namespace Renderers { namespace Utils {
    class Rectangle;
}}}
namespace Client {
    class Client;
    class ActionBinder;
    namespace Menu { namespace Widget {
        class Button;
    }}
}
namespace Tools {
    class IRenderer;
}

namespace Client { namespace Menu {

    class Menu;

    class MainMenu
    {
    private:
        Client& _client;
        Menu& _menu;
        Tools::IRenderer& _renderer;
        glm::detail::tmat4x4<float> _backRectMatrix;
        Tools::Renderers::Utils::Rectangle* _backRect;
        int _callbackId;
        bool _visible;
        ActionBinder* _actionBinder;
        Widget::Button* _quitButton;
        Widget::Button* _backButton;

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
