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

    public:
        Window(InputManager* inputManager);
        virtual ~Window();
        virtual void Render() = 0;
        virtual Tools::IRenderer& GetRenderer() = 0;
        InputManager& GetInputManager();
        virtual Tools::Vector2u const& GetSize() const = 0;
        virtual void Resize(Tools::Vector2u const& size) = 0;
        virtual void Resize(unsigned int w, unsigned int h) = 0;
    };

}}

#endif
