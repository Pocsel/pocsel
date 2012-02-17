#ifndef __CLIENT_WINDOW_INPUTMANAGER_HPP__
#define __CLIENT_WINDOW_INPUTMANAGER_HPP__

#include "client/ActionBinder.hpp"
#include "tools/Vector2.hpp"

namespace Client { namespace Window {
    class Window;
}}

namespace Client { namespace Window {

    class InputManager :
        public ActionBinder
    {
    private:
        InputBinder* _inputBinder;
        std::list<std::pair<InputBinder::Action, BindAction::Type>> _actionList;
        bool _actionStates[BindAction::NbBindActions];

    protected:
        bool _hasFocus;
        Window& _window;

    public:
        InputManager(::Client::Window::Window& window, InputBinder* inputBinder);
        virtual ~InputManager();
        virtual void ProcessEvents() = 0;
        void FlushActionList();
        void TriggerAction(InputBinder::Action const& action, BindAction::Type type);
        void TriggerAction(std::string const& action, BindAction::Type type);
        void TriggerAction(BindAction::BindAction action, BindAction::Type type);
        virtual Tools::Vector2i const& GetMousePos() const = 0;
        virtual void WarpMouse(Tools::Vector2i const& pos) = 0;
        virtual void WarpMouse(int x, int y) = 0;
        virtual void ShowMouse(bool show = true) = 0;
        virtual bool HasFocus() const = 0;

        std::list<std::pair<InputBinder::Action, BindAction::Type>>& GetActionList() { return this->_actionList; }
        InputBinder& GetInputBinder() { return *this->_inputBinder; }
        bool GetActionState(BindAction::BindAction action) { return this->_actionStates[static_cast<int>(action)]; }

    protected:
        void _ResetStates();
    };

}}

#endif
