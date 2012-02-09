#ifndef __CLIENT_WINDOW_SDL_INPUTMANAGER_HPP__
#define __CLIENT_WINDOW_SDL_INPUTMANAGER_HPP__

#include "client2/window/IInputManager.hpp"

namespace Client {
    class Client;
}

namespace Client { namespace Window { namespace Sdl {

    class InputManager :
        public IInputManager
    {
    public:
        InputManager(Client& client);
        virtual void ProcessEvents();
    };

}}}

#endif
