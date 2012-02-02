#if 0

# include <stdexcept>
# include <cstring>

# include "tools/renderers/GLRenderer.hpp"
# ifdef Expose
#  error "bite"
# endif
# include "WindowLinux.hpp"

using namespace Tools::Gui;

GLint WindowImplem::_attrs[] = {
    GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None,
};

WindowImplem::WindowImplem(std::string const& title, size_t width, size_t height)
    : _hideCursor(false)
{
    if ((this->_display = ::XOpenDisplay(0)) == 0)
        throw std::runtime_error("Cannot connect to the X server");
    this->_root = DefaultRootWindow(this->_display);
    this->_visual_info = ::glXChooseVisual(
        this->_display, 0, this->_attrs
    );

    if (this->_visual_info == 0)
        throw std::runtime_error("Cannot found appropriate visual");
    this->_colormap = ::XCreateColormap(
        this->_display,
        this->_root,
        this->_visual_info->visual,
        AllocNone
    );
    this->_set_window_attributes.colormap = this->_colormap;
    this->_set_window_attributes.event_mask =
        ExposureMask |
        KeyPressMask |
        StructureNotifyMask | //ResizeRedirectMask |
        ButtonReleaseMask | ButtonPressMask |
        PointerMotionMask;
    this->_window = ::XCreateWindow(
        this->_display,
        this->_root,
        40, 40, width, height,
        0, this->_visual_info->depth,
        InputOutput,
        this->_visual_info->visual,
        CWColormap | CWEventMask,
        &this->_set_window_attributes
    );
    ::XMapWindow(this->_display, this->_window);
    ::XStoreName(this->_display, this->_window, title.c_str());

    this->_gl_context = glXCreateContext(
        this->_display,
        this->_visual_info,
        NULL, GL_TRUE
    );
    ::glXMakeCurrent(
        this->_display,
        this->_window,
        this->_gl_context
    );
    Atom wmDelete = ::XInternAtom(
        this->_display, "WM_DELETE_WINDOW", True
    );
    ::XSetWMProtocols(
        this->_display, this->_window, &wmDelete, 1
    );

    // vsync
    const char *extensions = (char*)glXQueryExtensionsString(this->_display, this->_visual_info->screen);//(char*)glGetString(GL_EXTENSIONS);
    if (strstr(extensions, "GLX_SGI_swap_control") != 0)
    {
        bool (*glXSwapIntervalSGI)(int) = 0;
        glXSwapIntervalSGI = (bool(*)(int))glXGetProcAddress((const GLubyte*)"glXSwapIntervalSGI");

        if (glXSwapIntervalSGI)
            glXSwapIntervalSGI(1);
    }

    this->_renderer = new Tools::Renderers::GLRenderer();
    this->_renderer->Initialise();
}

WindowImplem::~WindowImplem()
{
    Delete(this->_renderer);
    ::glXMakeCurrent(this->_display, None, NULL);
    ::glXDestroyContext(this->_display, this->_gl_context);
    ::XDestroyWindow(this->_display, this->_window);
    ::XCloseDisplay(this->_display);
}

Tools::Vector2<size_t> const& WindowImplem::GetSize(bool force_refresh)
{
    if (force_refresh)
    {
        ::XWindowAttributes window_attributes;
        ::XGetWindowAttributes(this->_display, this->_window, &window_attributes);
        this->_size.w = window_attributes.width;
        this->_size.h = window_attributes.height;
    }
    return this->_size;
}

void WindowImplem::SwapBuffers()
{
    ::glXSwapBuffers(this->_display, this->_window);
}

void WindowImplem::Render()
{
    this->SwapBuffers();
}

void WindowImplem::WarpCursor(unsigned int x, unsigned int y)
{
    XWarpPointer(this->_display, None, this->_window, 0, 0, 0, 0, x, y);
}

void WindowImplem::CenterCursor()
{
    this->WarpCursor(this->_size.x / 2, this->_size.y / 2);
}

void WindowImplem::ShowCursor()
{
    if (!this->_hideCursor)
        return;
    this->_hideCursor = false;

    XUndefineCursor(this->_display, this->_window);
}

void WindowImplem::HideCursor()
{
    if (this->_hideCursor)
        return;
    this->_hideCursor = true;

    Cursor invisibleCursor;
    Pixmap bitmapNoData;
    XColor black;
    const char noData[] = { 0,0,0,0,0,0,0,0 };
    black.red = black.green = black.blue = 0;

    bitmapNoData = XCreateBitmapFromData(this->_display, this->_window, noData, 8, 8);
    invisibleCursor = XCreatePixmapCursor(this->_display, bitmapNoData, bitmapNoData,
            &black, &black, 0, 0);
    XDefineCursor(this->_display, this->_window, invisibleCursor);
    XFreeCursor(this->_display, invisibleCursor);
}

#endif
