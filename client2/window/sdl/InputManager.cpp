#include "client2/window/sdl/InputManager.hpp"
#include "client2/window/sdl/InputBinder.hpp"
#include "client2/Client.hpp"
#include "client2/window/Window.hpp"

namespace Client { namespace Window { namespace Sdl {

    InputManager::InputManager(Client& client, InputBinder* inputBinder) :
        ::Client::Window::InputManager(client, inputBinder), _inputBinder(*inputBinder), _mousePos(0)
    {
    }

    void InputManager::ProcessEvents()
    {
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
                        this->_TriggerBind(k, InputType::Pressed);
                        this->_keysHeld[e.key.keysym.sym] = k;
                    }
                    break;
                case SDL_KEYUP:
                    {
                        auto it = this->_keysHeld.find(e.key.keysym.sym);
                        if (it != this->_keysHeld.end())
                        {
                            this->_TriggerBind(it->second, InputType::Released);
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
                        this->_TriggerBind(b, InputType::Pressed);
                        this->_buttonsHeld[e.button.button] = b;
                    }
                    break;
                case SDL_MOUSEBUTTONUP:
                    {
                        auto it = this->_buttonsHeld.find(e.button.button);
                        if (it != this->_buttonsHeld.end())
                        {
                            this->_TriggerBind(it->second, InputType::Released);
                            this->_buttonsHeld.erase(it);
                        }
                        else
                            Tools::debug << "InputManager: Button up " << e.button.button << " with no button down.\n";
                    }
                    break;
                case SDL_VIDEORESIZE:
                    this->_client.GetWindow().Resize(e.resize.w, e.resize.h);
                    break;
                case SDL_QUIT:
                    this->_client.Quit();
                    break;
            }
        // repeat keys
        {
            auto it = this->_keysHeld.begin();
            auto itEnd = this->_keysHeld.end();
            for (; it != itEnd; ++it)
                this->_TriggerBind(it->second, InputType::Held);
        }
        // repeat buttons
        {
            auto it = this->_buttonsHeld.begin();
            auto itEnd = this->_buttonsHeld.end();
            for (; it != itEnd; ++it)
                this->_TriggerBind(it->second, InputType::Held);
        }
    }

    Tools::Vector2<int> const& InputManager::GetMousePos() const
    {
        return this->_mousePos;
    }

    void InputManager::WarpMouse(Tools::Vector2<int> const& pos)
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

    void InputManager::_TriggerBind(KeyHeld const& k, InputType::InputType type)
    {
    }

    void InputManager::_TriggerBind(ButtonHeld const& b, InputType::InputType type)
    {
        InputBinder::Action a;
        if (this->_inputBinder.GetButtonAction(b.button, a))
            this->TriggerAction(a, type);
    }

}}}
