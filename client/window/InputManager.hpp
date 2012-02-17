#ifndef __CLIENT_WINDOW_INPUTMANAGER_HPP__
#define __CLIENT_WINDOW_INPUTMANAGER_HPP__

#include "client/window/InputBinder.hpp"
#include "tools/Vector2.hpp"

namespace Client { namespace Window {
    class Window;
}}

namespace Client { namespace Window {

    class InputManager :
        private boost::noncopyable
    {
    private:
        InputBinder* _inputBinder;
        BindAction::Type _actionStates[BindAction::NbBindActions];
        std::queue<std::pair<InputBinder::Action, BindAction::Type>> _actionQueue;
        std::map<std::string, std::list<std::function<void(void)>>> _stringPressBinds;
        std::map<std::string, std::list<std::function<void(void)>>> _stringHoldBinds;
        std::map<std::string, std::list<std::function<void(void)>>> _stringReleaseBinds;
        std::map<BindAction::BindAction, std::list<std::function<void(void)>>> _pressBinds;
        std::map<BindAction::BindAction, std::list<std::function<void(void)>>> _holdBinds;
        std::map<BindAction::BindAction, std::list<std::function<void(void)>>> _releaseBinds;

    protected:
        bool _hasFocus;
        Window& _window;

    public:
        InputManager(::Client::Window::Window& window, InputBinder* inputBinder);
        virtual ~InputManager();
        virtual void ProcessEvents() = 0;
        void DispatchActions();
        InputBinder& GetInputBinder();
        void Bind(std::string const& action, BindAction::Type type, std::function<void(void)> const& func);
        void Bind(BindAction::BindAction action, BindAction::Type type, std::function<void(void)> const& func);
        void TriggerAction(InputBinder::Action const& action, BindAction::Type type);
        void TriggerAction(std::string const& action, BindAction::Type type);
        void TriggerAction(BindAction::BindAction action, BindAction::Type type);
        virtual Tools::Vector2i const& GetMousePos() const = 0;
        virtual void WarpMouse(Tools::Vector2i const& pos) = 0;
        virtual void WarpMouse(int x, int y) = 0;

        bool HasFocus() const { return this->_hasFocus; }
        BindAction::Type GetActionState(BindAction::BindAction action) { return this->_actionStates[(int)action]; }
    };

}}

#endif
