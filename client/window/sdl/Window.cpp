#include "client/precompiled.hpp"

#include "client/window/sdl/Window.hpp"
#include "client/window/sdl/InputManager.hpp"
#include "client/window/sdl/InputBinder.hpp"
#include "tools/renderers/DX9Renderer.hpp"
#include "tools/renderers/GLRenderer.hpp"
#include "ProgramInfo.hpp"

namespace Client { namespace Window { namespace Sdl {

    Window::Window(bool directX,
            glm::uvec2 const& size /* = glm::uvec2(800, 600) */,
            bool fullscreen /* = false */,
            bool useShaders /* = true */) :
        ::Client::Window::Window(new InputManager(*this, new InputBinder())),
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
        if (!(this->_screen = SDL_SetVideoMode(this->_size.w, this->_size.h, 0, (fullscreen ? SDL_FULLSCREEN : SDL_RESIZABLE) | rendererFlag)))
        {
            SDL_Quit();
            throw std::runtime_error(std::string("SDL_SetVideoMode(): ") + SDL_GetError());
        }
        SDL_WM_SetCaption(PROJECT_NAME, 0);
        SDL_EnableUNICODE(SDL_ENABLE);
#ifdef _WIN32
        if (directX)
            this->_renderer = new Tools::Renderers::DX9Renderer(this->_size, fullscreen);
        else
            this->_renderer = new Tools::Renderers::GLRenderer(useShaders);
#else
        (void)directX;
        this->_renderer = new Tools::Renderers::GLRenderer(useShaders);
#endif
        this->_renderer->Initialise();
        this->_renderer->SetScreenSize(this->_size);
        this->_renderer->SetViewport(Tools::Rectangle(glm::ivec2(0), this->_size));
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
            ((Tools::Renderers::DX9Renderer*)this->_renderer)->Present();
        else
            SDL_GL_SwapBuffers();
#else
        SDL_GL_SwapBuffers();
#endif
        if (this->_targetSize.w && this->_targetSize.h)
        {
            if (!(this->_screen = SDL_SetVideoMode(this->_targetSize.w, this->_targetSize.h, 0, SDL_RESIZABLE | SDL_OPENGL)))
            {
                SDL_Quit();
                throw std::runtime_error(SDL_GetError());
            }
            this->_size = this->_targetSize;
            this->_renderer->SetScreenSize(this->_size);
            this->_renderer->SetViewport(Tools::Rectangle(glm::ivec2(0), this->_size));
            this->_OnResize(this->_size);
            this->_targetSize.w = 0;
            this->_targetSize.h = 0;
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
