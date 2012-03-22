#include "server/precompiled.hpp"

#include "server/game/engine/MessageManager.hpp"
#include "server/game/engine/EntityManager.hpp"
#include "server/game/engine/Engine.hpp"
#include "tools/lua/Interpreter.hpp"
#include "tools/lua/Ref.hpp"

namespace Server { namespace Game { namespace Engine {

    MessageManager::MessageManager(Engine& engine) :
        _engine(engine)
    {
        Tools::debug << "MessageManager::MessageManager()\n";
        auto& i = this->_engine.GetInterpreter();
        auto namespaceTable = i.Globals()["Server"].Set("Message", i.MakeTable());
        namespaceTable.Set("Later", i.MakeFunction(std::bind(&MessageManager::_ApiLater, this, std::placeholders::_1)));
        namespaceTable.Set("Now", i.MakeFunction(std::bind(&MessageManager::_ApiNow, this, std::placeholders::_1)));
    }

    MessageManager::~MessageManager()
    {
        Tools::debug << "MessageManager::~MessageManager()\n";
        auto it = this->_messages.begin();
        auto itEnd = this->_messages.end();
        for (; it != itEnd; ++it)
        {
            auto itMessage = it->second.begin();
            auto itMessageEnd = it->second.end();
            for (; itMessage != itMessageEnd; ++itMessage)
                Tools::Delete(*itMessage);
        }
    }

    void MessageManager::DispatchMessages()
    {
        auto it = this->_messages.begin();
        auto itEnd = this->_messages.end();
        for (; it != itEnd; ++it)
        {
            if (it->first > this->_engine.GetCurrentTime())
                break;
            auto itMessage = it->second.begin();
            auto itMessageEnd = it->second.end();
            for (; itMessage != itMessageEnd; ++itMessage)
            {
                CallbackManager::Result res = this->_engine.GetCallbackManager().TriggerCallback((*itMessage)->callbackId);
                if ((*itMessage)->notificationCallbackId)
                {
                    auto resultTable = this->_engine.GetInterpreter().MakeTable();
                    resultTable.Set("entityId", this->_engine.GetInterpreter().MakeNumber((*itMessage)->targetId));
                    if (res == CallbackManager::Ok || res == CallbackManager::Error)
                        resultTable.Set("success", this->_engine.GetInterpreter().MakeBoolean(true));
                    else
                        resultTable.Set("success", this->_engine.GetInterpreter().MakeBoolean(false));
                    this->_engine.GetCallbackManager().TriggerCallback((*itMessage)->notificationCallbackId, resultTable);
                }
                Tools::Delete(*itMessage);
            }
        }
        this->_messages.erase(this->_messages.begin(), it);
    }

    void MessageManager::_ApiLater(Tools::Lua::CallHelper& helper)
    {
        double seconds = helper.PopArg().CheckNumber("Server.Message.Later: Argument 1 (seconds) must be a number");
        if (seconds < 0)
            seconds = 0;
        Uint32 targetId = (Uint32)helper.PopArg().CheckNumber("Server.Message.Later: Argument 2 (target) must be a number");
        std::string function = helper.PopArg().CheckString("Server.Message.Later: Argument 3 (function) must be a string");
        Tools::Lua::Ref arg(this->_engine.GetInterpreter().GetState());
        Uint32 cbTargetId = 0;
        std::string cbFunction;
        Tools::Lua::Ref cbArg(this->_engine.GetInterpreter().GetState());
        if (helper.GetNbArgs())
        {
            arg = helper.PopArg();
            if (helper.GetNbArgs())
            {
                cbTargetId = (Uint32)helper.PopArg().CheckNumber("Server.Message.Later: Argument 5 (cbTarget) must be a number");
                cbFunction = helper.PopArg().CheckString("Server.Message.Later: Argument 6 (cbFunction) must be a string");
                if (helper.GetNbArgs())
                    cbArg = helper.PopArg();
            }
        }
        Uint32 callbackId = this->_engine.GetCallbackManager().MakeCallback(targetId, function, arg);
        Uint32 notificationCallbackId = 0;
        if (cbTargetId)
            notificationCallbackId = this->_engine.GetCallbackManager().MakeCallback(cbTargetId, cbFunction, cbArg);
        this->_messages[this->_engine.GetCurrentTime() + (Uint64)(seconds * 1000000.0)].push_back(new Message(targetId, callbackId, notificationCallbackId));
    }

    void MessageManager::_ApiNow(Tools::Lua::CallHelper& helper)
    {
        helper.GetArgList().push_front(this->_engine.GetInterpreter().MakeNumber(0));
        this->_ApiLater(helper);
    }

}}}

