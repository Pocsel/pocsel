#ifndef __CLIENT_MENU_LOADGINGSCREEN_HPP__
#define __CLIENT_MENU_LOADGINGSCREEN_HPP__

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

    class LoadingScreen
    {
    private:
        Client& _client;
        Menu& _menu;
        Tools::IRenderer& _renderer;
        Tools::Matrix4<float> _text1Matrix;
        Tools::Matrix4<float> _text2Matrix;
        Tools::Renderers::Utils::Rectangle* _backRect;
        Tools::Renderers::Utils::Rectangle* _barRect;

    public:
        LoadingScreen(Client& client);
        ~LoadingScreen();
        void Render(std::string const& status, float progress);
    private:
        void _Resize(Tools::Vector2u const& size);
    };

}}

#endif
