#ifndef __CLIENT_WINDOW_WINDOW_HPP__
#define __CLIENT_WINDOW_WINDOW_HPP__

#include "tools/Vector2.hpp"

namespace Tools {
    class IRenderer;
}

namespace Client { namespace Window {

    class InputManager;

    class Window :
        private boost::noncopyable
    {
    private:
        InputManager* _inputManager;
        int _callbackNextId;
        std::map<int, std::function<void(Tools::Vector2u const&)>> _resizeCallbacks;

    public:
        Window(InputManager* inputManager);
        virtual ~Window();
        virtual void Render() = 0;
        virtual Tools::IRenderer& GetRenderer() = 0;
        InputManager& GetInputManager() { return *this->_inputManager; }
        virtual Tools::Vector2u const& GetSize() const = 0;
        virtual void Resize(Tools::Vector2u const& size) = 0;
        virtual void Resize(unsigned int w, unsigned int h) = 0;
        int RegisterCallback(std::function<void(Tools::Vector2u const&)> const& callback);
        void UnregisterCallback(int callbackId);
    protected:
        void _OnResize(Tools::Vector2u const& newSize);
    };

}}

#endif
