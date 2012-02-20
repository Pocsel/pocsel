#ifndef __CLIENT_ACTIONBINDER_HPP__
#define __CLIENT_ACTIONBINDER_HPP__

#include "client/BindAction.hpp"
#include "client/window/InputBinder.hpp"

namespace Client { namespace Window {
    class InputManager;
}}

namespace Client {

    class ActionBinder :
        private boost::noncopyable
    {
    private:
        std::map<std::string, std::list<std::function<void(void)>>> _stringPressBinds;
        std::map<std::string, std::list<std::function<void(void)>>> _stringHoldBinds;
        std::map<std::string, std::list<std::function<void(void)>>> _stringReleaseBinds;
        std::map<BindAction::BindAction, std::list<std::function<void(void)>>> _pressBinds;
        std::map<BindAction::BindAction, std::list<std::function<void(void)>>> _holdBinds;
        std::map<BindAction::BindAction, std::list<std::function<void(void)>>> _releaseBinds;
    public:
        virtual ~ActionBinder() {}
        void Dispatch(Window::InputManager& inputManager, bool catchAll = false);
        void Bind(std::string const& action, BindAction::Type type, std::function<void(void)> const& func);
        void Bind(BindAction::BindAction action, BindAction::Type type, std::function<void(void)> const& func);
    };

}

#endif
