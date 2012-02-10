#ifndef __CLIENT_WINDOW_SDL_INPUTMANAGER_HPP__
#define __CLIENT_WINDOW_SDL_INPUTMANAGER_HPP__

#include "client2/window/InputManager.hpp"
#include "tools/Vector2.hpp"

namespace Client {
    class Client;
}

namespace Client { namespace Window { namespace Sdl {

    class InputBinder;

    class InputManager :
        public ::Client::Window::InputManager
    {
    private:
        struct KeyHeld
        {
            char ascii;
            Uint16 unicode;
            SpecialKey::SpecialKey specialKey;
        };
        struct ButtonHeld
        {
            Button::Button button;
        };

    private:
        InputBinder& _inputBinder;
        std::map<SDLKey, KeyHeld> _keysHeld;
        std::map<int, ButtonHeld> _buttonsHeld;
        Tools::Vector2i _mousePos;

    public:
        InputManager(Client& client, InputBinder* inputBinder);
        virtual void ProcessEvents();
        virtual Tools::Vector2i const& GetMousePos() const;
        virtual void WarpMouse(Tools::Vector2i const& pos);
        virtual void WarpMouse(int x, int y);
    private:
        char _UnicodeToAscii(Uint16 unicode) const;
        void _TriggerBind(KeyHeld const& k, BindAction::Type type);
        void _TriggerBind(ButtonHeld const& b, BindAction::Type type);
    };

}}}

#endif
