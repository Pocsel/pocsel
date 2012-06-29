#ifndef __CLIENT_MENU_LOADGINGSCREEN_HPP__
#define __CLIENT_MENU_LOADGINGSCREEN_HPP__

namespace Tools {
    namespace Renderers { namespace Utils {
        class Rectangle;
    }}
    class IRenderer;
}
namespace Client {
    class Client;
}

namespace Client { namespace Menu {

    class Menu;

    class LoadingScreen
    {
    private:
        Client& _client;
        Menu& _menu;
        Tools::IRenderer& _renderer;
        glm::detail::tmat4x4<float> _text1Matrix;
        glm::detail::tmat4x4<float> _text2Matrix;
        glm::detail::tmat4x4<float> _backRectMatrix;
        Tools::Renderers::Utils::Rectangle* _backRect;
        Tools::Renderers::Utils::Rectangle* _barRect;
        int _callbackId;

    public:
        LoadingScreen(Client& client, Menu& menu);
        ~LoadingScreen();
        void Render(std::string const& status, float progress);
    private:
        void _Resize(glm::uvec2 const& size);
    };

}}

#endif
