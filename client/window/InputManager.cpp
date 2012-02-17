#include "client/precompiled.hpp"

#include "client/window/InputManager.hpp"
#include "client/window/InputBinder.hpp"

namespace {

    template <typename T, typename U>
        void CallFunctions(T& map, U const& action)
        {
            auto list = map[action];
            auto it = list.begin();
            auto itEnd = list.end();
            for (; it != itEnd; ++it)
                (*it)();
        }

}

namespace Client { namespace Window {

    InputManager::InputManager(::Client::Window::Window& window, InputBinder* inputBinder) :
        _inputBinder(inputBinder),
        _hasFocus(true),
        _window(window)
    {
        this->_ResetStates();
    }

    InputManager::~InputManager()
    {
        delete this->_inputBinder;
    }

    void InputManager::_ResetStates()
    {
        for (unsigned int i = 0; i < BindAction::NbBindActions; ++i)
            this->_actionStates[i] = false;
    }

    void InputManager::DispatchActions()
    {
        this->_ResetStates();
        while (!this->_actionQueue.empty())
        {
            auto a = this->_actionQueue.front();
            if (a.first.string.empty())
            {
                switch (a.second)
                {
                case BindAction::Pressed:
                    CallFunctions(this->_pressBinds, a.first.action);
                    break;
                case BindAction::Held:
                    CallFunctions(this->_holdBinds, a.first.action);
                    break;
                case BindAction::Released:
                    CallFunctions(this->_releaseBinds, a.first.action);
                    break;
                }
                this->_actionStates[static_cast<int>(a.first.action)] = true;
            }
            else
                switch (a.second)
                {
                case BindAction::Pressed:
                    CallFunctions(this->_stringPressBinds, a.first.string);
                    break;
                case BindAction::Held:
                    CallFunctions(this->_stringHoldBinds, a.first.string);
                    break;
                case BindAction::Released:
                    CallFunctions(this->_stringReleaseBinds, a.first.string);
                    break;
                }
            this->_actionQueue.pop();
        }
    }

    void InputManager::Bind(std::string const& action, BindAction::Type type, std::function<void(void)> const& func)
    {
        switch (type)
        {
        case BindAction::Pressed:
            this->_stringPressBinds[action].push_back(func);
            break;
        case BindAction::Held:
            this->_stringHoldBinds[action].push_back(func);
            break;
        case BindAction::Released:
            this->_stringReleaseBinds[action].push_back(func);
            break;
        }
    }

    void InputManager::Bind(BindAction::BindAction action, BindAction::Type type, std::function<void(void)> const& func)
    {
        switch (type)
        {
        case BindAction::Pressed:
            this->_pressBinds[action].push_back(func);
            break;
        case BindAction::Held:
            this->_holdBinds[action].push_back(func);
            break;
        case BindAction::Released:
            this->_releaseBinds[action].push_back(func);
            break;
        }
    }

    void InputManager::TriggerAction(InputBinder::Action const& action, BindAction::Type type)
    {
        this->_actionQueue.push(std::make_pair(action, type));
    }

    void InputManager::TriggerAction(std::string const& action, BindAction::Type type)
    {
        InputBinder::Action a;
        a.string = action;
        this->TriggerAction(a, type);
    }

    void InputManager::TriggerAction(BindAction::BindAction action, BindAction::Type type)
    {
        InputBinder::Action a;
        a.action = action;
        this->TriggerAction(a, type);
    }

}}
