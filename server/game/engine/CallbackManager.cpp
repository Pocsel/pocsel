#include "server/game/engine/CallbackManager.hpp"
#include "server/game/engine/Engine.hpp"
#include "server/game/engine/EntityManager.hpp"
#include "tools/lua/Interpreter.hpp"

namespace Server { namespace Game { namespace Engine {

    CallbackManager::CallbackManager(Engine& engine) :
        _engine(engine),
        _nextCallbackId(1) // la première callback sera la 1, 0 est la valeur spéciale "pas de callback"
    {
    }

    CallbackManager::~CallbackManager()
    {
        auto it = this->_callbacks.begin();
        auto itEnd = this->_callbacks.end();
        for (; it != itEnd; ++it)
            Tools::Delete(it->second);
    }

    Uint32 CallbackManager::MakeCallback(Uint32 targetId, std::string const& function, Tools::Lua::Ref const& arg, bool serialize /* = true */)
    {
        while (!this->_nextCallbackId // 0 est la valeur spéciale "pas de callback", on la saute
                && this->_callbacks.count(this->_nextCallbackId))
            ++this->_nextCallbackId;
        this->_callbacks[this->_nextCallbackId] = new Callback(targetId, function,
                serialize ? this->_engine.GetInterpreter().GetSerializer().MakeSerializableCopy(arg, true) : arg);
        return this->_nextCallbackId++;
    }

    CallbackManager::Result CallbackManager::TriggerCallback(Uint32 callbackId, Tools::Lua::Ref* ret /* = 0 */, bool keepCallback /* = false */)
    {
        return this->TriggerCallback(callbackId, this->_engine.GetInterpreter().MakeNil(), ret, keepCallback);
    }

    CallbackManager::Result CallbackManager::TriggerCallback(Uint32 callbackId, Tools::Lua::Ref const& bonusArg, Tools::Lua::Ref* ret /* = 0 */, bool keepCallback /* = false */)
    {
        if (!callbackId)
            return Ok;
        auto it = this->_callbacks.find(callbackId);
        if (it != this->_callbacks.end())
        {
            Result res = this->_engine.GetEntityManager().CallEntityFunction(it->second->targetId, it->second->function, it->second->arg, bonusArg, ret);
            if (!keepCallback)
            {
                delete it->second;
                this->_callbacks.erase(it);
            }
            return res;
        }
        Tools::error << "CallbackManager::TriggerCallback: Callback " << callbackId << " not found.\n";
        return CallbackNotFound;
    }

    bool CallbackManager::CancelCallback(Uint32 callbackId)
    {
        if (!callbackId)
            return true;
        auto it = this->_callbacks.find(callbackId);
        if (it != this->_callbacks.end())
        {
            delete it->second;
            this->_callbacks.erase(it);
            return true;
        }
        Tools::error << "CallbackManager::CancelCallback: Callback " << callbackId << " not found.\n";
        return false;
    }

}}}
