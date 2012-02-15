#ifndef __CLIENT_MENU_DISCONNECTEDSCREEN_HPP__
#define __CLIENT_MENU_DISCONNECTEDSCREEN_HPP__

#include "tools/Matrix4.hpp"

namespace Tools { namespace Renderers { namespace Utils {
    class Rectangle;
}}}
namespace Client {
    class Client;
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
        Tools::Matrix4<float> _text1Matrix;
        Tools::Matrix4<float> _text2Matrix;
        Tools::Matrix4<float> _backRectMatrix;
        Tools::Renderers::Utils::Rectangle* _backRect;
        int _callbackId;

    public:
        DisconnectedScreen(Client& client, Menu& menu);
        ~DisconnectedScreen();
        void Render(std::string const& message);
    private:
        void _Resize(Tools::Vector2u const& size);
    };

}}

#endif
