#ifndef __CLIENT_WINDOW_SDL_WINDOW_HPP__
#define __CLIENT_WINDOW_SDL_WINDOW_HPP__

#include "tools/window/Window.hpp"
#include "tools/Vector2.hpp"

namespace Tools {
    class IRenderer;
}

namespace Tools { namespace Window { namespace Sdl {

    class Window :
        public ::Tools::Window::Window
    {
    private:
        Tools::IRenderer* _renderer;
        SDL_Surface* _screen;
        glm::uvec2 _size;
        glm::uvec2 _targetSize;
#ifdef _WIN32
        bool _directX;
#endif

    public:
        Window(bool directX = false, glm::uvec2 const& size = glm::uvec2(800, 600), bool fullscreen = false, bool useShaders = true);
        ~Window();
        virtual void Render();
        virtual Tools::IRenderer& GetRenderer() { return *this->_renderer; }
        virtual glm::uvec2 const& GetSize() const { return this->_size; }
        virtual void Resize(glm::uvec2 const& size);
        virtual void Resize(unsigned int w, unsigned int h);
    };

}}}

#endif
