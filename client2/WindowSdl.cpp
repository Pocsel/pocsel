#include "client2/WindowSdl.hpp"
#include "tools/renderers/GLRenderer.hpp"

namespace Client {

    WindowSdl::WindowSdl(Client& client)
    {
        if (SDL_Init(SDL_INIT_VIDEO))
            throw std::runtime_error(std::string("SDL_Init(): ") + SDL_GetError());
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 1);
        if (!(this->_screen = SDL_SetVideoMode(800, 600, 0, SDL_RESIZABLE | SDL_OPENGL)))
        {
            SDL_Quit();
            throw std::runtime_error(std::string("SDL_SetVideoMode(): ") + SDL_GetError());
        }
        SDL_WM_SetCaption("Pocsel", 0);
        SDL_EnableUNICODE(SDL_ENABLE);
        this->_renderer = new Tools::Renderers::GLRenderer();
        this->_renderer->Initialise();
    }

    WindowSdl::~WindowSdl()
    {
        SDL_Quit();
        Tools::Delete(this->_renderer);
    }

    void WindowSdl::Render()
    {
        SDL_GL_SwapBuffers();
    }

}
