#include "client2/window/sdl/InputManager.hpp"
#include "client2/window/sdl/InputBinder.hpp"

namespace Client { namespace Window { namespace Sdl {

    InputManager::InputManager(Client& client, InputBinder* inputBinder) :
        IInputManager(client, inputBinder), _inputBinder(*inputBinder)
    {
    }

    void InputManager::ProcessEvents()
    {
        Action action;
        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            switch (e.type)
            {
                case SDL_MOUSEMOTION:
                    break;
                case SDL_KEYDOWN:
                    KeyHeld k;
                    k.ascii = this->_UnicodeToAscii(e.key.keysym.unicode);
                    k.unicode = e.key.keysym.unicode;
                    k.specialKey = this->_inputBinder.SdlKeysymToSpecialKey(e.key.keysym.sym);
                    this->_TriggerBind(k, InputType::Pressed);
                    this->_keysHeld[e.key.keysym.sym] = k;
                    break;
                case SDL_KEYUP:
                    auto it = this->_keysHeld.find(e.key.keysym.sym);
                    if (it != this->_keysHeld.end())
                    {
                        this->_TriggerBind(it->second, InputType::Released);
                        this->_keysHeld.erase(it);
                    }
                    else
                        Tools::debug << "InputManager: Key up " << e.key.keysym.sym << " with no key down.\n";
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    ButtonHeld b;
                    b.button = this->_inputBinder.SdlButtonToButton(e.button.button);
                    b.x = e.button.x;
                    b.y = e.button.y;
                    this->_TriggerBind(b, InputType::Pressed);
                    this->_buttonsHeld[e.button.button] = b;
                    break;
                case SDL_MOUSEBUTTONUP:
                    auto it = this->_buttonsHeld.find(e.key.keysym.sym);
                    if (it != this->_buttonsHeld.end())
                    {
                        this->_TriggerBind(it->second, InputType::Released);
                        this->_buttonsHeld.erase(it);
                    }
                    break;
                case SDL_VIDEORESIZE:
                    break;
                case SDL_QUIT:
                    this->_client.Quit();
                    break;
                default:
                    ;
            }
        }
        // repeat keys
        {
            this->_keysHeld.unique();
            auto it = this->_keysHeld.begin();
            auto itEnd = this->_keysHeld.end();
            for (; it != itEnd; ++it)
                this->_TriggerBind(it->second, InputType::Held);
        }
        // repeat buttons
        {
            this->_buttonsHeld.unique();
            auto it = this->_buttonsHeld.begin();
            auto itEnd = this->_buttonsHeld.end();
            for (; it != itEnd; ++it)
                this->_TriggerBind(it->second, InputType::Held);
        }
        return 0;
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
    }

}}}
