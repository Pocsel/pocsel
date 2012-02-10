#include "client2/window/InputManager.hpp"
#include "client2/window/InputBinder.hpp"

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

    InputManager::InputManager(Client& client, InputBinder* inputBinder) :
        _inputBinder(inputBinder), _client(client)
    {
    }

    InputManager::~InputManager()
    {
        delete this->_inputBinder;
    }

    void InputManager::DispatchActions()
    {
        while (!this->_actionQueue.empty())
        {
            auto a = this->_actionQueue.front();
            if (a.first.string.empty())
                switch (a.second)
                {
                    case InputType::Pressed:
                        CallFunctions(this->_pressBinds, a.first.action);
                        break;
                    case InputType::Held:
                        CallFunctions(this->_holdBinds, a.first.action);
                        break;
                    case InputType::Released:
                        CallFunctions(this->_releaseBinds, a.first.action);
                        break;
                }
            else
                switch (a.second)
                {
                    case InputType::Pressed:
                        CallFunctions(this->_stringPressBinds, a.first.string);
                        break;
                    case InputType::Held:
                        CallFunctions(this->_stringHoldBinds, a.first.string);
                        break;
                    case InputType::Released:
                        CallFunctions(this->_stringReleaseBinds, a.first.string);
                        break;
                }
            this->_actionQueue.pop();
        }
    }

    InputBinder& InputManager::GetInputBinder()
    {
        return *this->_inputBinder;
    }

    void InputManager::Bind(std::string const& action, InputType::InputType type, std::function<void(void)> const& func)
    {
        switch (type)
        {
            case InputType::Pressed:
                this->_stringPressBinds[action].push_back(func);
                break;
            case InputType::Held:
                this->_stringHoldBinds[action].push_back(func);
                break;
            case InputType::Released:
                this->_stringReleaseBinds[action].push_back(func);
                break;
        }
    }

    void InputManager::Bind(BindAction::BindAction action, InputType::InputType type, std::function<void(void)> const& func)
    {
        switch (type)
        {
            case InputType::Pressed:
                this->_pressBinds[action].push_back(func);
                break;
            case InputType::Held:
                this->_holdBinds[action].push_back(func);
                break;
            case InputType::Released:
                this->_releaseBinds[action].push_back(func);
                break;
        }
    }

    void InputManager::TriggerAction(InputBinder::Action const& action, InputType::InputType type)
    {
        this->_actionQueue.push(std::make_pair(action, type));
    }

    void InputManager::TriggerAction(std::string const& action, InputType::InputType type)
    {
        InputBinder::Action a;
        a.string = action;
        this->TriggerAction(a, type);
    }

    void InputManager::TriggerAction(BindAction::BindAction action, InputType::InputType type)
    {
        InputBinder::Action a;
        a.action = action;
        this->TriggerAction(a, type);
    }

}}
