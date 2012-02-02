
#include <iostream>

#include "Application.hpp"
#include "Event.hpp"
#include "events/Quit.hpp"
#include "tools/gui/Window.hpp"

using namespace Tools::Gui;

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

