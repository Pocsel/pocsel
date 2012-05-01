#include "tools/precompiled.hpp"

#include "tools/window/sdl/InputManager.hpp"
#include "tools/window/sdl/InputBinder.hpp"
#include "tools/window/Window.hpp"

namespace Tools { namespace Window { namespace Sdl {

    InputManager::InputManager(::Tools::Window::Window& window, InputBinder* inputBinder) :
        ::Tools::Window::InputManager(window, inputBinder),
        _inputBinder(*inputBinder),
        _mousePos(0),
        _showMouse(false),
        _mouseShown(true),
        _hasFocus(true),
        _isMinimized(false)
    {
    }

    void InputManager::ProcessEvents()
    {
        this->_ResetStates();
        InputBinder::Action action;
        SDL_Event e;
        while (SDL_PollEvent(&e))
            switch (e.type)
            {
            case SDL_MOUSEMOTION:
                this->_mousePos.x = e.motion.x;
                this->_mousePos.y = e.motion.y;
                break;
            case SDL_KEYDOWN:
                {
                    KeyHeld k;
                    k.ascii = this->_UnicodeToAscii(e.key.keysym.unicode);
                    k.unicode = e.key.keysym.unicode;
                    k.specialKey = this->_inputBinder.SdlKeysymToSpecialKey(e.key.keysym.sym);
                    this->_TriggerBind(k, BindAction::Pressed);
                    this->_keysHeld[e.key.keysym.sym] = k;
                }
                break;
            case SDL_KEYUP:
                {
                    auto it = this->_keysHeld.find(e.key.keysym.sym);
                    if (it != this->_keysHeld.end())
                    {
                        this->_TriggerBind(it->second, BindAction::Released);
                        this->_keysHeld.erase(it);
                    }
                    else
                        Tools::debug << "InputManager: Key up " << e.key.keysym.sym << " with no key down.\n";
                }
                break;
            case SDL_MOUSEBUTTONDOWN:
                {
                    ButtonHeld b;
                    b.button = this->_inputBinder.SdlButtonToButton(e.button.button);
                    this->_TriggerBind(b, BindAction::Pressed);
                    this->_buttonsHeld[e.button.button] = b;
                }
                break;
            case SDL_MOUSEBUTTONUP:
                {
                    auto it = this->_buttonsHeld.find(e.button.button);
                    if (it != this->_buttonsHeld.end())
                    {
                        this->_TriggerBind(it->second, BindAction::Released);
                        this->_buttonsHeld.erase(it);
                    }
                    else
                        Tools::debug << "InputManager: Button up " << e.button.button << " with no button down.\n";
                }
                break;
            case SDL_ACTIVEEVENT:
                if ((e.active.state & SDL_APPINPUTFOCUS) == SDL_APPINPUTFOCUS)
                    this->_hasFocus = e.active.gain != 0;
                if ((e.active.state & SDL_APPACTIVE) == SDL_APPACTIVE)
                    this->_isMinimized = e.active.gain == 0;
                break;
            case SDL_VIDEORESIZE:
                this->_window.Resize(e.resize.w, e.resize.h);
                break;
            case SDL_QUIT:
                this->TriggerAction(BindAction::Quit, BindAction::Released);
                break;
            default:
                ;
            }
        // repeat keys
        {
            auto it = this->_keysHeld.begin();
            auto itEnd = this->_keysHeld.end();
            for (; it != itEnd; ++it)
                this->_TriggerBind(it->second, BindAction::Held);
        }
        // repeat buttons
        {
            auto it = this->_buttonsHeld.begin();
            auto itEnd = this->_buttonsHeld.end();
            for (; it != itEnd; ++it)
                this->_TriggerBind(it->second, BindAction::Held);
        }
        // mouse
        if (this->_showMouse)
            this->_ShowMouse(true);
        else
            this->_ShowMouse(false);
        this->_showMouse = false;
    }

    glm::ivec2 InputManager::GetMousePosRealTime() const
    {
        glm::ivec2 ret;
        SDL_PumpEvents();
        SDL_GetMouseState(&ret.x, &ret.y);
        return ret;
    }

    void InputManager::ShowMouse()
    {
        this->_showMouse = true;
    }

    void InputManager::_ShowMouse(bool show)
    {
        if (show && !this->_mouseShown)
            SDL_ShowCursor(SDL_ENABLE);
        else if (!show && this->_mouseShown)
            SDL_ShowCursor(SDL_DISABLE);
        this->_mouseShown = show;
    }

    void InputManager::WarpMouse(glm::detail::tvec2<int> const& pos)
    {
        this->WarpMouse(pos.x, pos.y);
    }

    void InputManager::WarpMouse(int x, int y)
    {
        SDL_WarpMouse(x, y);
        this->_mousePos.x = x;
        this->_mousePos.y = y;
    }

    char InputManager::_UnicodeToAscii(Uint16 unicode) const
    {
        if (unicode != 0 && (unicode & 0xFF80) == 0)
            return unicode & 0x7F;
        return 0;
    }

    void InputManager::_TriggerBind(KeyHeld const& k, BindAction::Type type)
    {
        InputBinder::Action a;
        if (k.ascii && this->_inputBinder.GetAsciiAction(k.ascii, a))
            this->TriggerAction(a, type);
        else if (this->_inputBinder.GetSpecialKeyAction(k.specialKey, a))
            this->TriggerAction(a, type);
        else if (k.unicode && this->_inputBinder.GetUnicodeAction(k.unicode, a))
            this->TriggerAction(a, type);
    }

    void InputManager::_TriggerBind(ButtonHeld const& b, BindAction::Type type)
    {
        InputBinder::Action a;
        if (this->_inputBinder.GetButtonAction(b.button, a))
            this->TriggerAction(a, type);
    }

}}}
