#include "tools/precompiled.hpp"

#include "tools/window/sdl/Window.hpp"
#include "tools/window/sdl/InputManager.hpp"
#include "tools/window/sdl/InputBinder.hpp"
#include "tools/gfx/DX9Renderer.hpp"
#include "tools/gfx/GLRenderer.hpp"
#include "ProgramInfo.hpp"

using namespace Tools::Gfx;

namespace Tools { namespace Window { namespace Sdl {

    Window::Window(std::map<std::string, Tools::Window::BindAction::BindAction> const& actions,
            bool directX,
            glm::uvec2 const& size,
            bool fullscreen) :
        ::Tools::Window::Window(new InputManager(*this, new InputBinder(actions))),
        _size(size),
        _targetSize(0)
#ifdef _WIN32
        ,_directX(directX)
#endif
    {
        if (SDL_Init(SDL_INIT_VIDEO))
            throw std::runtime_error(std::string("SDL_Init(): ") + SDL_GetError());
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 1);
        Uint32 rendererFlag = SDL_OPENGL;
#ifdef _WIN32
        rendererFlag = directX ? 0 : SDL_OPENGL;
#endif
        rendererFlag |= fullscreen ? SDL_FULLSCREEN : SDL_RESIZABLE;
        this->_rendererFlag = rendererFlag;
        if (!(this->_screen = SDL_SetVideoMode(this->_size.x, this->_size.y, 32, this->_rendererFlag)))
        {
            SDL_Quit();
            throw std::runtime_error(std::string("SDL_SetVideoMode(): ") + SDL_GetError());
        }
        SDL_WM_SetCaption(PROJECT_NAME, 0);
        SDL_EnableUNICODE(SDL_ENABLE);
#ifdef _WIN32
        if (directX)
            this->_renderer = new DX9Renderer(this->_size, fullscreen);
        else
            this->_renderer = new GLRenderer(this->_size, fullscreen);
#else
        (void)directX;
        this->_renderer = new GLRenderer(this->_size, fullscreen);
#endif
        this->_renderer->Initialise();
        this->_renderer->SetScreenSize(this->_size);
        //this->_renderer->SetViewport(Tools::Rectangle(glm::ivec2(0), this->_size));
    }

    Window::~Window()
    {
        Tools::Delete(this->_renderer);
        SDL_Quit();
    }

    void Window::Render()
    {
#ifdef _WIN32
        if (this->_directX)
            ((DX9Renderer*)this->_renderer)->Present();
        else
            SDL_GL_SwapBuffers();
#else
        SDL_GL_SwapBuffers();
#endif
        if (this->_targetSize.x && this->_targetSize.y)
        {
#ifndef _WIN32
            if (!(this->_screen = SDL_SetVideoMode(this->_targetSize.x, this->_targetSize.y, 0, this->_rendererFlag)))
            {
                SDL_Quit();
                throw std::runtime_error(SDL_GetError());
            }
#endif
            this->_size = this->_targetSize;
            this->_renderer->SetScreenSize(this->_size);
            //this->_renderer->SetViewport(Tools::Rectangle(glm::ivec2(0), this->_size));
            this->_OnResize(this->_size);
            this->_targetSize.x = 0;
            this->_targetSize.y = 0;
        }
    }

    void Window::Resize(glm::uvec2 const& size)
    {
        this->_targetSize = size;
    }

    void Window::Resize(unsigned int w, unsigned int h)
    {
        this->Resize(glm::uvec2(w, h));
    }

}}}
