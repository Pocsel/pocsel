#ifndef __CLIENT_WINDOW_SDL_INPUTBINDER_HPP__
#define __CLIENT_WINDOW_SDL_INPUTBINDER_HPP__

#include "client2/window/InputBinder.hpp"

namespace Client { namespace Window { namespace Sdl {

    class InputBinder :
        public Window::InputBinder
    {
        private:
            std::map<SDLKey, SpecialKey::SpecialKey> _specialKeys;
            std::map<int, Button::Button> _buttons;

        public:
            InputBinder();
            bool GetSdlSpecialKeyAction(SDLKey specialKey, Action& action);
            bool GetSdlButtonAction(int button, Action& action);
            SpecialKey::SpecialKey SdlKeysymToSpecialKey(SDLKey keysym) const;
            Button::Button SdlButtonToButton(SDLKey keysym) const;
        private:
            void _PopulateSpecialKeys();
            void _PopulateButtons();
    };

}}}

#endif
