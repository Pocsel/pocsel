#ifndef __CLIENT_WINDOW_IWINDOW_HPP__
#define __CLIENT_WINDOW_IWINDOW_HPP__

#include <boost/noncopyable.hpp>

namespace Client { namespace Window {

    class IInputManager;

    class IWindow :
        private boost::noncopyable
    {
    private:
        IInputManager* _inputManager;

    public:
        IWindow(IInputManager* inputManager);
        virtual ~IWindow();
        virtual void Render() = 0;
        IInputManager& GetInputManager();
    };

}}

#endif
