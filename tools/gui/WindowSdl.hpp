#ifndef __TOOLS_GUI_WINDOWSDL_HPP__
#define __TOOLS_GUI_WINDOWSDL_HPP__

#include <GL/glew.h>

#include <SDL/SDL.h>

#include "tools/IRenderer.hpp"
#include "tools/Vector2.hpp"

namespace Tools { namespace Gui {

    class WindowImplem
    {
    private:
        IRenderer* _renderer;
        SDL_Surface* _screen;
        Vector2<size_t> _targetSize;
        bool _resizeAtNextFrame;

    public:
        WindowImplem(std::string const& title, size_t width, size_t height);
        virtual ~WindowImplem();
        void Render();
        IRenderer& GetRenderer() { return *this->_renderer; }
        size_t GetWidth() const { return this->_targetSize.w; }
        size_t GetHeight() const { return this->_targetSize.h; }
        Vector2<size_t> const& GetSize(bool force_refresh = false);
        void WarpCursor(unsigned int x, unsigned int y);
        void CenterCursor();
        void ShowCursor();
        void HideCursor();
        void Resize(Vector2<size_t> newSize);
    };

}}

#endif
