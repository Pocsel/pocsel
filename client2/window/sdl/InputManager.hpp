#ifndef __CLIENT_WINDOW_SDL_INPUTMANAGER_HPP__
#define __CLIENT_WINDOW_SDL_INPUTMANAGER_HPP__

#include "client2/window/IInputManager.hpp"

namespace Client {
    class Client;
}

namespace Client { namespace Window { namespace Sdl {

    class InputBinder;

    class InputManager :
        public IInputManager
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
            int x;
            int y;
        };

    private:
        InputBinder& _inputBinder;
        std::map<SDLKey, KeyHeld> _keysHeld;
        std::map<int, ButtonHeld> _buttonsHeld;

    public:
        InputManager(Client& client);
        virtual void ProcessEvents();
    private:
        char _UnicodeToAscii(Uint16 unicode) const;
        void _TriggerBind(KeyHeld const& k, InputType::InputType type);
        void _TriggerBind(ButtonHeld const& b, InputType::InputType type);
    };

}}}

#endif
