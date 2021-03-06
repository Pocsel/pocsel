#include "tools/precompiled.hpp"
#include "tools/window/Window.hpp"
#include "tools/window/InputManager.hpp"

namespace Tools { namespace Window {

    Window::Window(InputManager* inputManager) :
        _inputManager(inputManager),
        _callbackNextId(0)
    {
    }

    Window::~Window()
    {
        Tools::Delete(this->_inputManager);
    }

    int Window::RegisterCallback(std::function<void(glm::uvec2 const&)> const& callback)
    {
        this->_resizeCallbacks[this->_callbackNextId] = callback;
        return this->_callbackNextId++;
    }

    void Window::UnregisterCallback(int callbackId)
    {
        this->_resizeCallbacks.erase(callbackId);
    }

    void Window::_OnResize(glm::uvec2 const& newSize)
    {
        for (auto it = this->_resizeCallbacks.begin(), ite = this->_resizeCallbacks.end(); it != ite; ++it)
            it->second(newSize);
    }

}}
