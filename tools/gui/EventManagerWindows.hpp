#ifndef __TOOLS_GUI_EVENTMANAGERWINDOWS_HPP__
#define __TOOLS_GUI_EVENTMANAGERWINDOWS_HPP__

#include "tools/gui/WindowWindows.hpp"

namespace Tools { namespace Gui {

    class Window;
    struct Event;

    class EventManagerImplem
    {
    private:
        Window& _window;
        WindowImplem::Callback _windowCallback;

    public:
        EventManagerImplem(Window& window);
        ~EventManagerImplem();
        unsigned int Poll();
        void Stop();

    private:
        void _Fire(std::string const& id, Event* ev);
        long _WinProc(WindowImplem& window, UINT msg, WPARAM wParam, LPARAM lParam);
    };

}}

#endif
