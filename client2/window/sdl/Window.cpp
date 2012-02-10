#include "client2/window/sdl/Window.hpp"
#include "client2/window/sdl/InputManager.hpp"
#include "tools/renderers/GLRenderer.hpp"
#include "ProgramInfo.hpp"

namespace Client { namespace Window { namespace Sdl {

    Window::Window(Client& client) :
        IWindow(new InputManager(client, new InputBinder))
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
        SDL_WM_SetCaption(PROJECT_NAME, 0);
        SDL_EnableUNICODE(SDL_ENABLE);
        this->_renderer = new Tools::Renderers::GLRenderer();
        this->_renderer->Initialise();
    }

    Window::~Window()
    {
        SDL_Quit();
        Tools::Delete(this->_renderer);
    }

    void Window::Render()
    {
        SDL_GL_SwapBuffers();
    }

}}}
