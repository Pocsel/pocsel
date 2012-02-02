#ifndef __TOOLS_GUI_EVENTMANAGERLINUX_HPP__
#define __TOOLS_GUI_EVENTMANAGERLINUX_HPP__

namespace Tools { namespace Gui {

    class Window;
    class Event;

    class EventManagerImplem
    {
    private:
        Window& _window;

    public:
        EventManagerImplem(Window& window);
        ~EventManagerImplem();
        unsigned int Poll();
        void Stop();

    private:
        void _Fire(std::string const& id, Event const& ev);
    };

}}

#endif
