#ifndef __CLIENT_WINDOWSDL_HPP__
#define __CLIENT_WINDOWSDL_HPP__

#include <SDL/SDL.h>

#include "client2/IWindow.hpp"

namespace Tools {
    class IRenderer;
}

namespace Client {

    class Client;

    class WindowSdl :
        public IWindow
    {
        private:
            Tools::IRenderer* _renderer;
            SDL_Surface* _screen;

        public:
            WindowSdl(Client& client);
            ~WindowSdl();
            virtual void Render();
    };

}

#endif
