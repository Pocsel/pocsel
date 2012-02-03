#include <iostream>
#include <stdexcept>

#include "WindowSdl.hpp"
#include "tools/renderers/GLRenderer.hpp"

namespace Tools { namespace Gui {

    WindowImplem::WindowImplem(std::string const& title, size_t width, size_t height, bool useShaders) :
        _resizeAtNextFrame(false)
    {
        if (SDL_Init(SDL_INIT_VIDEO))
            throw std::runtime_error(SDL_GetError());
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 1);
        if (!(this->_screen = SDL_SetVideoMode(width, height, 0, SDL_RESIZABLE | SDL_OPENGL)))
        {
            SDL_Quit();
            throw std::runtime_error(SDL_GetError());
        }
        this->_targetSize.w = width;
        this->_targetSize.h = height;
        SDL_WM_SetCaption(title.c_str(), 0);
        //SDL_EnableKeyRepeat(130, 35);
        SDL_EnableUNICODE(SDL_ENABLE);
        this->_renderer = new Renderers::GLRenderer(useShaders);
        this->_renderer->Initialise();
    }

    WindowImplem::~WindowImplem()
    {
        SDL_Quit();
        Delete(this->_renderer);
    }

    void WindowImplem::Render()
    {
        if (this->_resizeAtNextFrame)
        {
            this->_resizeAtNextFrame = false;
            if (!(this->_screen = SDL_SetVideoMode(this->_targetSize.w, this->_targetSize.h, 0, SDL_RESIZABLE | SDL_OPENGL)))
            {
                SDL_Quit();
                throw std::runtime_error(SDL_GetError());
            }
        }
        SDL_GL_SwapBuffers();
    }

    void WindowImplem::Resize(Vector2<size_t> newSize)
    {
        this->_targetSize = newSize;
        this->_resizeAtNextFrame = true;
    }

    Vector2<size_t> const& WindowImplem::GetSize(bool)
    {
        return this->_targetSize;
    }

    void WindowImplem::WarpCursor(unsigned int x, unsigned int y)
    {
        SDL_WarpMouse(x, y);
    }

    void WindowImplem::CenterCursor()
    {
        this->WarpCursor(this->GetWidth() / 2, this->GetHeight() / 2);
    }

    void WindowImplem::ShowCursor()
    {
        SDL_ShowCursor(SDL_ENABLE);
    }

    void WindowImplem::HideCursor()
    {
        SDL_ShowCursor(SDL_DISABLE);
    }

}}
