#include "server/precompiled.hpp"

#include "server/game/engine/CallManager.hpp"
#include "server/game/engine/EntityManager.hpp"
#include "server/game/engine/Engine.hpp"
#include "tools/lua/Interpreter.hpp"
#include "tools/lua/Ref.hpp"

namespace Server { namespace Game { namespace Engine {

    CallManager::CallManager(Engine& engine) :
        _engine(engine)
    {
        Tools::debug << "CallManager::CallManager()\n";
        auto& i = this->_engine.GetInterpreter();
        auto namespaceTable = i.Globals()["Server"].Set("Call", i.MakeTable());
        namespaceTable.Set("Later", i.MakeFunction(std::bind(&CallManager::_ApiLater, this, std::placeholders::_1)));
        namespaceTable.Set("Now", i.MakeFunction(std::bind(&CallManager::_ApiNow, this, std::placeholders::_1)));
    }

    CallManager::~CallManager()
    {
        Tools::debug << "CallManager::~CallManager()\n";
        auto it = this->_calls.begin();
        auto itEnd = this->_calls.end();
        for (; it != itEnd; ++it)
        {
            auto itCall = it->second.begin();
            auto itCallEnd = it->second.end();
            for (; itCall != itCallEnd; ++itCall)
                Tools::Delete(*itCall);
        }
    }

    void CallManager::DispatchCalls()
    {
        auto it = this->_calls.begin();
        auto itEnd = this->_calls.end();
        for (; it != itEnd; ++it)
        {
            if (it->first > this->_engine.GetCurrentTime())
                break;
            auto itCall = it->second.begin();
            auto itCallEnd = it->second.end();
            for (; itCall != itCallEnd; ++itCall)
            {
                CallbackManager::Result res = this->_engine.GetCallbackManager().TriggerCallback((*itCall)->callbackId);
                if ((*itCall)->notificationCallbackId)
                {
                    auto resultTable = this->_engine.GetInterpreter().MakeTable();
                    resultTable.Set("entityId", this->_engine.GetInterpreter().MakeNumber((*itCall)->targetId));
                    if (res == CallbackManager::Ok || res == CallbackManager::Error)
                        resultTable.Set("success", this->_engine.GetInterpreter().MakeBoolean(true));
                    else
                        resultTable.Set("success", this->_engine.GetInterpreter().MakeBoolean(false));
                    this->_engine.GetCallbackManager().TriggerCallback((*itCall)->notificationCallbackId, resultTable);
                }
                Tools::Delete(*itCall);
            }
        }
        this->_calls.erase(this->_calls.begin(), it);
    }

    void CallManager::_ApiLater(Tools::Lua::CallHelper& helper)
    {
        double seconds = helper.PopArg().CheckNumber("Server.Call.Later: Argument 1 (seconds) must be a number");
        if (seconds < 0)
            seconds = 0;
        Uint32 targetId = (Uint32)helper.PopArg().CheckNumber("Server.Call.Later: Argument 2 (target) must be a number");
        std::string function = helper.PopArg().CheckString("Server.Call.Later: Argument 3 (function) must be a string");
        Tools::Lua::Ref arg(this->_engine.GetInterpreter().GetState());
        Uint32 cbTargetId = 0;
        std::string cbFunction;
        Tools::Lua::Ref cbArg(this->_engine.GetInterpreter().GetState());
        if (helper.GetNbArgs())
        {
            arg = helper.PopArg();
            if (helper.GetNbArgs())
            {
                cbTargetId = (Uint32)helper.PopArg().CheckNumber("Server.Call.Later: Argument 5 (cbTarget) must be a number");
                cbFunction = helper.PopArg().CheckString("Server.Call.Later: Argument 6 (cbFunction) must be a string");
                if (helper.GetNbArgs())
                    cbArg = helper.PopArg();
            }
        }
        Uint32 callbackId = this->_engine.GetCallbackManager().MakeCallback(targetId, function, arg);
        Uint32 notificationCallbackId = 0;
        if (cbTargetId)
            notificationCallbackId = this->_engine.GetCallbackManager().MakeCallback(cbTargetId, cbFunction, cbArg);
        this->_calls[this->_engine.GetCurrentTime() + (Uint64)(seconds * 1000000.0)].push_back(new Call(targetId, callbackId, notificationCallbackId));
    }

    void CallManager::_ApiNow(Tools::Lua::CallHelper& helper)
    {
        helper.GetArgList().push_front(this->_engine.GetInterpreter().MakeNumber(0));
        this->_ApiLater(helper);
    }

}}}

