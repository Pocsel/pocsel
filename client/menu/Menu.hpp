#ifndef __CLIENT_MENU_MENU_HPP__
#define __CLIENT_MENU_MENU_HPP__

namespace Client {
    class Client;
}
namespace Tools { namespace Renderers {
    class IShaderProgram;
    class IShaderParameter;
    namespace Utils {
        class Font;
    }
}}

namespace Client { namespace Menu {

    class LoadingScreen;
    class DisconnectedScreen;
    class MainMenu;

    class Menu
    {
    private:
        Client& _client;
        Tools::Renderers::Utils::Font* _font;
        Tools::Renderers::IShaderProgram* _fontShader;
        Tools::Renderers::IShaderParameter* _fontColor;
        Tools::Renderers::IShaderParameter* _fontTexture;
        Tools::Renderers::IShaderProgram* _rectShader;
        LoadingScreen* _loadingScreen;
        DisconnectedScreen* _disconnectedScreen;
        MainMenu* _mainMenu;

    public:
        Menu(Client& client);
        ~Menu();
        void BeginMenuDrawing();
        void EndMenuDrawing();
        LoadingScreen& GetLoadingScreen() { return *this->_loadingScreen; }
        DisconnectedScreen& GetDisconnectedScreen() { return *this->_disconnectedScreen; }
        MainMenu& GetMainMenu() { return *this->_mainMenu; }
        Tools::Renderers::Utils::Font& GetFont() { return *this->_font; }
        Tools::Renderers::IShaderProgram& GetFontShader() { return *this->_fontShader; }
        Tools::Renderers::IShaderParameter& GetFontColor() { return *this->_fontColor; }
        Tools::Renderers::IShaderParameter& GetFontTexture() { return *this->_fontTexture; }
        Tools::Renderers::IShaderProgram& GetRectShader() { return *this->_rectShader; }
    };

}}

#endif
