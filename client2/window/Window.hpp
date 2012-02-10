#ifndef __CLIENT_WINDOW_WINDOW_HPP__
#define __CLIENT_WINDOW_WINDOW_HPP__

#include <boost/noncopyable.hpp>

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
    };

}}

#endif
