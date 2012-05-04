#include "tools/precompiled.hpp"

#include "tools/window/sdl/InputBinder.hpp"

namespace Tools { namespace Window { namespace Sdl {

    InputBinder::InputBinder(std::map<std::string, Tools::Window::BindAction::BindAction> const& actions) :
        Tools::Window::InputBinder(actions)
    {
        this->_PopulateSpecialKeys();
        this->_PopulateButtons();
    }

    bool InputBinder::GetSdlKeysymAction(SDLKey keysym, Action& action) const
    {
        return this->GetSpecialKeyAction(this->SdlKeysymToSpecialKey(keysym), action);
    }

    bool InputBinder::GetSdlButtonAction(int button, Action& action) const
    {
        return this->GetButtonAction(this->SdlButtonToButton(button), action);
    }

    SpecialKey::SpecialKey InputBinder::SdlKeysymToSpecialKey(SDLKey keysym) const
    {
        auto it = this->_specialKeys.find(keysym);
        if (it != this->_specialKeys.end())
            return it->second;
        return SpecialKey::None;
    }

    Button::Button InputBinder::SdlButtonToButton(int button) const
    {
        auto it = this->_buttons.find(button);
        if (it != this->_buttons.end())
            return it->second;
        return Button::None;
    }

    void InputBinder::_PopulateSpecialKeys()
    {
        this->_specialKeys[SDLK_BACKSPACE] = SpecialKey::Backspace;
        this->_specialKeys[SDLK_TAB] = SpecialKey::Tab;
        this->_specialKeys[SDLK_CLEAR] = SpecialKey::Clear;
        this->_specialKeys[SDLK_RETURN] = SpecialKey::Return;
        this->_specialKeys[SDLK_PAUSE] = SpecialKey::Pause;
        this->_specialKeys[SDLK_ESCAPE] = SpecialKey::Escape;
        this->_specialKeys[SDLK_SPACE] = SpecialKey::Space;
        this->_specialKeys[SDLK_DELETE] = SpecialKey::Delete;
        this->_specialKeys[SDLK_KP0] = SpecialKey::Kp0;
        this->_specialKeys[SDLK_KP1] = SpecialKey::Kp1;
        this->_specialKeys[SDLK_KP2] = SpecialKey::Kp2;
        this->_specialKeys[SDLK_KP3] = SpecialKey::Kp3;
        this->_specialKeys[SDLK_KP4] = SpecialKey::Kp4;
        this->_specialKeys[SDLK_KP5] = SpecialKey::Kp5;
        this->_specialKeys[SDLK_KP6] = SpecialKey::Kp6;
        this->_specialKeys[SDLK_KP7] = SpecialKey::Kp7;
        this->_specialKeys[SDLK_KP8] = SpecialKey::Kp8;
        this->_specialKeys[SDLK_KP9] = SpecialKey::Kp9;
        this->_specialKeys[SDLK_KP_PERIOD] = SpecialKey::KpPeriod;
        this->_specialKeys[SDLK_KP_DIVIDE] = SpecialKey::KpDivide;
        this->_specialKeys[SDLK_KP_MULTIPLY] = SpecialKey::KpMultiply;
        this->_specialKeys[SDLK_KP_MINUS] = SpecialKey::KpMinus;
        this->_specialKeys[SDLK_KP_PLUS] = SpecialKey::KpPlus;
        this->_specialKeys[SDLK_KP_ENTER] = SpecialKey::KpEnter;
        this->_specialKeys[SDLK_KP_EQUALS] = SpecialKey::KpEquals;
        this->_specialKeys[SDLK_UP] = SpecialKey::Up;
        this->_specialKeys[SDLK_DOWN] = SpecialKey::Down;
        this->_specialKeys[SDLK_RIGHT] = SpecialKey::Right;
        this->_specialKeys[SDLK_LEFT] = SpecialKey::Left;
        this->_specialKeys[SDLK_INSERT] = SpecialKey::Insert;
        this->_specialKeys[SDLK_HOME] = SpecialKey::Home;
        this->_specialKeys[SDLK_END] = SpecialKey::End;
        this->_specialKeys[SDLK_PAGEUP] = SpecialKey::Pageup;
        this->_specialKeys[SDLK_PAGEDOWN] = SpecialKey::Pagedown;
        this->_specialKeys[SDLK_F1] = SpecialKey::F1;
        this->_specialKeys[SDLK_F2] = SpecialKey::F2;
        this->_specialKeys[SDLK_F3] = SpecialKey::F3;
        this->_specialKeys[SDLK_F4] = SpecialKey::F4;
        this->_specialKeys[SDLK_F5] = SpecialKey::F5;
        this->_specialKeys[SDLK_F6] = SpecialKey::F6;
        this->_specialKeys[SDLK_F7] = SpecialKey::F7;
        this->_specialKeys[SDLK_F8] = SpecialKey::F8;
        this->_specialKeys[SDLK_F9] = SpecialKey::F9;
        this->_specialKeys[SDLK_F10] = SpecialKey::F10;
        this->_specialKeys[SDLK_F11] = SpecialKey::F11;
        this->_specialKeys[SDLK_F12] = SpecialKey::F12;
        this->_specialKeys[SDLK_F13] = SpecialKey::F13;
        this->_specialKeys[SDLK_F14] = SpecialKey::F14;
        this->_specialKeys[SDLK_F15] = SpecialKey::F15;
        this->_specialKeys[SDLK_NUMLOCK] = SpecialKey::Numlock;
        this->_specialKeys[SDLK_CAPSLOCK] = SpecialKey::Capslock;
        this->_specialKeys[SDLK_SCROLLOCK] = SpecialKey::Scrollock;
        this->_specialKeys[SDLK_RSHIFT] = SpecialKey::Rshift;
        this->_specialKeys[SDLK_LSHIFT] = SpecialKey::Lshift;
        this->_specialKeys[SDLK_RCTRL] = SpecialKey::Rctrl;
        this->_specialKeys[SDLK_LCTRL] = SpecialKey::Lctrl;
        this->_specialKeys[SDLK_RALT] = SpecialKey::Ralt;
        this->_specialKeys[SDLK_LALT] = SpecialKey::Lalt;
        this->_specialKeys[SDLK_RMETA] = SpecialKey::Rmeta;
        this->_specialKeys[SDLK_LMETA] = SpecialKey::Lmeta;
        this->_specialKeys[SDLK_LSUPER] = SpecialKey::Lsuper;
        this->_specialKeys[SDLK_RSUPER] = SpecialKey::Rsuper;
        this->_specialKeys[SDLK_MODE] = SpecialKey::AltGr;
        this->_specialKeys[SDLK_COMPOSE] = SpecialKey::Compose;
        this->_specialKeys[SDLK_HELP] = SpecialKey::Help;
        this->_specialKeys[SDLK_PRINT] = SpecialKey::Print;
        this->_specialKeys[SDLK_SYSREQ] = SpecialKey::Sysreq;
        this->_specialKeys[SDLK_BREAK] = SpecialKey::Break;
        this->_specialKeys[SDLK_MENU] = SpecialKey::Menu;
        this->_specialKeys[SDLK_POWER] = SpecialKey::Power;
        this->_specialKeys[SDLK_EURO] = SpecialKey::Euro;
        this->_specialKeys[SDLK_UNDO] = SpecialKey::Undo;
    }

    void InputBinder::_PopulateButtons()
    {
        this->_buttons[SDL_BUTTON_LEFT] = Button::Left;
        this->_buttons[SDL_BUTTON_MIDDLE] = Button::Middle;
        this->_buttons[SDL_BUTTON_RIGHT] = Button::Right;
        this->_buttons[SDL_BUTTON_WHEELUP] = Button::WheelUp;
        this->_buttons[SDL_BUTTON_WHEELDOWN] = Button::WheelDown;
        this->_buttons[SDL_BUTTON_X1] = Button::Special1;
        this->_buttons[SDL_BUTTON_X2] = Button::Special2;
    }

}}}
