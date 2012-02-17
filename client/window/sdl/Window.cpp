#include "client2/precompiled.hpp"

#include "client2/window/sdl/Window.hpp"
#include "client2/window/sdl/InputManager.hpp"
#include "client2/window/sdl/InputBinder.hpp"
#include "tools/renderers/GLRenderer.hpp"
#include "ProgramInfo.hpp"

namespace Client { namespace Window { namespace Sdl {

    Window::Window(Tools::Vector2u const& size, bool fullscreen, bool useShaders) :
        ::Client::Window::Window(new InputManager(*this, new InputBinder())),
        _size(size),
        _targetSize(0)
    {
        if (SDL_Init(SDL_INIT_VIDEO))
            throw std::runtime_error(std::string("SDL_Init(): ") + SDL_GetError());
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 1);
        if (!(this->_screen = SDL_SetVideoMode(this->_size.w, this->_size.h, 0, (fullscreen ? SDL_FULLSCREEN : SDL_RESIZABLE) | SDL_OPENGL)))
        {
            SDL_Quit();
            throw std::runtime_error(std::string("SDL_SetVideoMode(): ") + SDL_GetError());
        }
        SDL_WM_SetCaption(PROJECT_NAME, 0);
        SDL_EnableUNICODE(SDL_ENABLE);
        this->_renderer = new Tools::Renderers::GLRenderer(useShaders);
        this->_renderer->Initialise();
        this->_renderer->SetScreenSize(this->_size);
        this->_renderer->SetViewport(Tools::Rectangle(Tools::Vector2i(0), this->_size));
    }

    Window::~Window()
    {
        Tools::Delete(this->_renderer);
        SDL_Quit();
    }

    Tools::IRenderer& Window::GetRenderer()
    {
        return *this->_renderer;
    }

    void Window::Render()
    {
        SDL_GL_SwapBuffers();
        if (this->_targetSize.w && this->_targetSize.h)
        {
            if (!(this->_screen = SDL_SetVideoMode(this->_targetSize.w, this->_targetSize.h, 0, SDL_RESIZABLE | SDL_OPENGL)))
            {
                SDL_Quit();
                throw std::runtime_error(SDL_GetError());
            }
            this->_size = this->_targetSize;
            this->_renderer->SetScreenSize(this->_size);
            this->_renderer->SetViewport(Tools::Rectangle(Tools::Vector2i(0), this->_size));
            this->_OnResize(this->_size);
            this->_targetSize.w = 0;
            this->_targetSize.h = 0;
        }
    }

    void Window::Resize(Tools::Vector2u const& size)
    {
        this->_targetSize = size;
    }

    void Window::Resize(unsigned int w, unsigned int h)
    {
        this->Resize(Tools::Vector2u(w, h));
    }

    Tools::Vector2u const& Window::GetSize() const
    {
        return this->_size;
    }

}}}
