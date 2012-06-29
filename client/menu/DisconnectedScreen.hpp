#ifndef __CLIENT_MENU_DISCONNECTEDSCREEN_HPP__
#define __CLIENT_MENU_DISCONNECTEDSCREEN_HPP__

namespace Tools {
    namespace Renderers { namespace Utils {
        class Rectangle;
    }}
    namespace Window {
        class ActionBinder;
    }
}
namespace Client {
    class Client;
    namespace Menu { namespace Widget {
        class Button;
    }}
}
namespace Tools {
    class IRenderer;
}

namespace Client { namespace Menu {

    class Menu;

    class DisconnectedScreen
    {
    private:
        Client& _client;
        Menu& _menu;
        Tools::IRenderer& _renderer;
        glm::detail::tmat4x4<float> _text1Matrix;
        glm::detail::tmat4x4<float> _text2Matrix;
        glm::detail::tmat4x4<float> _backRectMatrix;
        Tools::Renderers::Utils::Rectangle* _backRect;
        int _callbackId;
        std::string _message;
        Tools::Window::ActionBinder* _actionBinder;
        Widget::Button* _button;

    public:
        DisconnectedScreen(Client& client, Menu& menu);
        ~DisconnectedScreen();
        void SetMessage(std::string const& message);
        void Render();
    private:
        void _Resize(glm::uvec2 const& size);
        void _RetryButton();
    };

}}

#endif
