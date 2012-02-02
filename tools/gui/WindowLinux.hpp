#ifndef  __TOOLS_GUI_WINDOWLINUX_HPP__
# define __TOOLS_GUI_WINDOWLINUX_HPP__

#ifdef __linux__

# ifdef Expose
#  error "Xlib already included"
# endif
# include <X11/X.h>
# include <X11/Xlib.h>
# include <GL/glx.h>

#include "tools/IRenderer.hpp"
#include "tools/Vector2.hpp"

namespace Tools { namespace Gui {

    class WindowImplem
    {
    private:
        ::Display*                  _display;
        ::Window                    _root;
        ::XVisualInfo*              _visual_info;
        ::Colormap                  _colormap;
        ::XSetWindowAttributes      _set_window_attributes;
        ::Window                    _window;
        ::GLXContext                _gl_context;
        Vector2<int>                _pos;
        Vector2<size_t>             _size;
        static ::GLint              _attrs[]; //TODO virer ça
        bool                        _hideCursor;

        IRenderer* _renderer;

    public:
        WindowImplem(std::string const& title, size_t width, size_t height);
        virtual ~WindowImplem();
        void Render();
        void SwapBuffers();

        IRenderer& GetRenderer() { return *this->_renderer; }
        ::Display* GetDisplay() { return this->_display; }
        size_t GetWidth() const { return this->_size.w; }
        size_t GetHeight() const { return this->_size.h; }
        Vector2<int> const& GetPos() const { return this->_pos; }
        Vector2<size_t> const& GetSize(bool force_refresh = false);
        void WarpCursor(unsigned int x, unsigned int y);
        void CenterCursor();
        void ShowCursor();
        void HideCursor();
    };

}}

#endif // __linux__

#endif
