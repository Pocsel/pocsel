#include "client/precompiled.hpp"
#include "client/window/Window.hpp"
#include "client/window/InputManager.hpp"

namespace Client { namespace Window {

    Window::Window(InputManager* inputManager) :
        _inputManager(inputManager),
        _callbackNextId(0)
    {
    }

    Window::~Window()
    {
        delete this->_inputManager;
    }

    int Window::RegisterCallback(std::function<void(Tools::Vector2u const&)> const& callback)
    {
        this->_resizeCallbacks[this->_callbackNextId] = callback;
        return this->_callbackNextId++;
    }

    void Window::UnregisterCallback(int callbackId)
    {
        this->_resizeCallbacks.erase(callbackId);
    }

    void Window::_OnResize(Tools::Vector2u const& newSize)
    {
        for (auto it = this->_resizeCallbacks.begin(), ite = this->_resizeCallbacks.end(); it != ite; ++it)
            it->second(newSize);
    }

}}
