#include "client/ActionBinder.hpp"
#include "client/window/InputManager.hpp"

namespace {
    template <typename T, typename U>
        void CallFunctions(T& map, U const& action, bool& bound)
        {
            auto list = map[action];
            auto it = list.begin();
            auto itEnd = list.end();
            for (; it != itEnd; ++it)
            {
                (*it)();
                bound = true;
            }
        }
}

namespace Client {

    void ActionBinder::Dispatch(Window::InputManager& inputManager, bool catchAll /* = false */)
    {
        bool bound;
        auto& actionList = inputManager.GetActionList();
        auto it = actionList.begin();
        while (it != actionList.end())
        {
            bound = false;
            if (it->first.string.empty())
            {
                switch (it->second)
                {
                case BindAction::Pressed:
                    CallFunctions(this->_pressBinds, it->first.action, bound);
                    break;
                case BindAction::Held:
                    CallFunctions(this->_holdBinds, it->first.action, bound);
                    break;
                case BindAction::Released:
                    CallFunctions(this->_releaseBinds, it->first.action, bound);
                    break;
                }
            }
            else
                switch (it->second)
                {
                case BindAction::Pressed:
                    CallFunctions(this->_stringPressBinds, it->first.string, bound);
                    break;
                case BindAction::Held:
                    CallFunctions(this->_stringHoldBinds, it->first.string, bound);
                    break;
                case BindAction::Released:
                    CallFunctions(this->_stringReleaseBinds, it->first.string, bound);
                    break;
                }
            if (bound)
                actionList.erase(it++);
            else
                ++it;
        }
        if (catchAll)
            actionList.clear();
    }

    void ActionBinder::Bind(std::string const& action, BindAction::Type type, std::function<void(void)> const& func)
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

    void ActionBinder::Bind(BindAction::BindAction action, BindAction::Type type, std::function<void(void)> const& func)
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

}
