#include "client2/window/IWindow.hpp"
#include "client2/window/IInputManager.hpp"

namespace Client { namespace Window {

    IWindow::IWindow(IInputManager* inputManager) :
        _inputManager(inputManager)
    {
    }

    IWindow::~IWindow()
    {
        delete this->_inputManager;
    }

    IInputManager& IWindow::GetInputManager()
    {
        return *this->_inputManager;
    }

}}
