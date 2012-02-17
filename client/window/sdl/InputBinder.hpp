#ifndef __CLIENT_WINDOW_SDL_INPUTBINDER_HPP__
#define __CLIENT_WINDOW_SDL_INPUTBINDER_HPP__

#include "client/window/InputBinder.hpp"

namespace Client { namespace Window { namespace Sdl {

    class InputBinder :
        public ::Client::Window::InputBinder
    {
        private:
            std::map<SDLKey, SpecialKey::SpecialKey> _specialKeys;
            std::map<int, Button::Button> _buttons;

        public:
            InputBinder();
            bool GetSdlKeysymAction(SDLKey keysym, Action& action) const;
            bool GetSdlButtonAction(int button, Action& action) const;
            SpecialKey::SpecialKey SdlKeysymToSpecialKey(SDLKey keysym) const;
            Button::Button SdlButtonToButton(int button) const;
        private:
            void _PopulateSpecialKeys();
            void _PopulateButtons();
    };

}}}

#endif
