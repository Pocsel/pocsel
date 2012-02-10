#ifndef __TOOLS_GUI_APPLICATION_HPP__
#define __TOOLS_GUI_APPLICATION_HPP__

#include "tools/gui/EventManager.hpp"
#include "tools/gui/Settings.hpp"

namespace Tools { namespace Gui {

    class Application : private boost::noncopyable
    {
    protected:
        Window& _mainWindow;
        EventManager& _eventManager;
        Settings _settings;

    public:
        Application(Window& window);
        virtual ~Application() {}

        virtual int Run(int ac, char *av[]) = 0;
        virtual void Stop();

        EventManager& GetEventManager() { return this->_eventManager; }
        Settings& GetSettings() { return this->_settings; }
    protected:
        virtual void _OnStop(Event const&) { this->Stop(); }
    };

}}

#endif
