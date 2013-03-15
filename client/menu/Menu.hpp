#ifndef __CLIENT_MENU_MENU_HPP__
#define __CLIENT_MENU_MENU_HPP__

namespace Client {
    class Client;
}
namespace Tools { namespace Gfx {
    class IShaderParameter;
    namespace Effect {
        class Effect;
    }
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
        Tools::Gfx::Utils::Font* _font;
        Tools::Gfx::Effect::Effect* _fontShader;
        Tools::Gfx::IShaderParameter* _fontColor;
        Tools::Gfx::IShaderParameter* _fontTexture;
        Tools::Gfx::Effect::Effect* _rectShader;
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
        Tools::Gfx::Utils::Font& GetFont() { return *this->_font; }
        Tools::Gfx::Effect::Effect& GetFontShader() { return *this->_fontShader; }
        Tools::Gfx::IShaderParameter& GetFontColor() { return *this->_fontColor; }
        Tools::Gfx::IShaderParameter& GetFontTexture() { return *this->_fontTexture; }
        Tools::Gfx::Effect::Effect& GetRectShader() { return *this->_rectShader; }
    };

}}

#endif
