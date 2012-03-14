#include "server/game/engine/CallbackManager.hpp"
#include "server/game/engine/Engine.hpp"
#include "server/game/engine/EntityManager.hpp"

namespace Server { namespace Game { namespace Engine {

    CallbackManager::CallbackManager(Engine& engine) :
        _engine(engine), _currentCallbackId(0)
    {
    }

    CallbackManager::~CallbackManager()
    {
    }

    Uint32 CallbackManager::MakeCallback(int targetId, std::string const& function, Tools::Lua::Ref const& arg, Tools::Lua::Ref const& bonusArg)
    {
        while (this->_callbacks.count(this->_currentCallbackId))
            ++this->_currentCallbackId;
        this->_callbacks[this->_currentCallbackId] = new Callback(targetId, function, arg, bonusArg);
        return this->_currentCallbackId++;
    }

    bool CallbackManager::TriggerCallback(Uint32 callbackId)
    {
        auto it = this->_callbacks.find(callbackId);
        if (it != this->_callbacks.end())
        {
            this->_engine.GetEntityManager().TriggerCallback(*it->second);
            delete it->second;
            this->_callbacks.erase(it);
            return true;
        }
        return false;
    }

    bool CallbackManager::CancelCallback(Uint32 callbackId)
    {
        auto it = this->_callbacks.find(callbackId);
        if (it != this->_callbacks.end())
        {
            delete it->second;
            this->_callbacks.erase(it);
            return true;
        }
        return false;
    }

}}}
