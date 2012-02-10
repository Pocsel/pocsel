#include "tools/gui/Application.hpp"
#include "tools/gui/Event.hpp"
#include "tools/gui/Window.hpp"

#include "tools/gui/events/Quit.hpp"

namespace Tools { namespace Gui {

    Application::Application(Window& window) :
        _mainWindow(window),
        _eventManager(window.GetEventManager())
    {
    }

    void Application::Stop()
    {
        std::cout << "Application::Stop()\n";
        this->_eventManager.Notify<Events::Quit>("on-quit");
    }

}}
