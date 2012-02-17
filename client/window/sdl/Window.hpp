#ifndef __CLIENT_WINDOW_SDL_WINDOW_HPP__
#define __CLIENT_WINDOW_SDL_WINDOW_HPP__

#include "client/window/Window.hpp"
#include "tools/Vector2.hpp"

namespace Tools {
    class IRenderer;
}

namespace Client { namespace Window { namespace Sdl {

    class Window :
        public ::Client::Window::Window
    {
    private:
        Tools::IRenderer* _renderer;
        SDL_Surface* _screen;
        Tools::Vector2u _size;
        Tools::Vector2u _targetSize;

    public:
        Window(Tools::Vector2u const& size = Tools::Vector2u(800, 600), bool fullscreen = false, bool useShaders = true);
        ~Window();
        virtual void Render();
        virtual Tools::IRenderer& GetRenderer();
        virtual Tools::Vector2u const& GetSize() const;
        virtual void Resize(Tools::Vector2u const& size);
        virtual void Resize(unsigned int w, unsigned int h);
    };

}}}

#endif
