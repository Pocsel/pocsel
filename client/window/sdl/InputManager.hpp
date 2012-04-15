#ifndef __CLIENT_WINDOW_SDL_INPUTMANAGER_HPP__
#define __CLIENT_WINDOW_SDL_INPUTMANAGER_HPP__

#include "client/window/InputManager.hpp"
#include "tools/Vector2.hpp"

namespace Client { namespace Window {
    class Window;
}}

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
        glm::ivec2 _mousePos;
        bool _showMouse;
        bool _mouseShown;
        bool _hasFocus;
        bool _isMinimized;

    public:
        InputManager(::Client::Window::Window& window, InputBinder* inputBinder);
        virtual void ProcessEvents();
        virtual glm::ivec2 const& GetMousePos() const { return this->_mousePos; }
        virtual glm::ivec2 GetMousePosRealTime() const;
        virtual void WarpMouse(glm::ivec2 const& pos);
        virtual void WarpMouse(int x, int y);
        virtual void ShowMouse();
        virtual bool MouseShown() const { return this->_showMouse; }
        virtual bool HasFocus() const { return this->_hasFocus; }
        virtual bool IsMinimized() const { return this->_isMinimized; }
    private:
        char _UnicodeToAscii(Uint16 unicode) const;
        void _TriggerBind(KeyHeld const& k, BindAction::Type type);
        void _TriggerBind(ButtonHeld const& b, BindAction::Type type);
        void _ShowMouse(bool show);
    };

}}}

#endif
