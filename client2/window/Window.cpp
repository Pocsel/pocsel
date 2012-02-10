#include "client2/window/Window.hpp"
#include "client2/window/InputManager.hpp"

namespace Client { namespace Window {

    Window::Window(InputManager* inputManager) :
        _inputManager(inputManager)
    {
    }

    Window::~Window()
    {
        delete this->_inputManager;
    }

    InputManager& Window::GetInputManager()
    {
        return *this->_inputManager;
    }

}}
