#ifndef __CLIENT_WINDOW_SDL_WINDOW_HPP__
#define __CLIENT_WINDOW_SDL_WINDOW_HPP__

#include <SDL/SDL.h>

#include "client2/window/IWindow.hpp"

namespace Tools {
    class IRenderer;
}
namespace Client {
    class Client;
}

namespace Client { namespace Window { namespace Sdl {

    class Window :
        public IWindow
    {
        private:
            Tools::IRenderer* _renderer;
            SDL_Surface* _screen;

        public:
            Window(Client& client);
            ~Window();
            virtual void Render();
    };

}}}

#endif
