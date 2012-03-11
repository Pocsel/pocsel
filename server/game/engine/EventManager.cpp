#include "server/game/engine/EventManager.hpp"
#include "server/game/engine/EntityManager.hpp"
#include "server/game/engine/Engine.hpp"
#include "tools/lua/Interpreter.hpp"
#include "tools/lua/Ref.hpp"

namespace Server { namespace Game { namespace Engine {

    EventManager::EventManager(Engine& engine) :
        _engine(engine)
    {
        Tools::debug << "EventManager::EventManager()\n";
        auto& i = this->_engine.GetInterpreter();
        auto eventTable = i.Globals()["Server"].Set("Event", i.MakeTable());
        eventTable.Set("CallLater", i.MakeFunction(std::bind(&EventManager::_CallLater, this, std::placeholders::_1)));
    }

    EventManager::~EventManager()
    {
        Tools::debug << "EventManager::~EventManager()\n";
        std::for_each(this->_events.begin(), this->_events.end(), [](std::pair<Uint64 const, std::list<Event*>>& it)
                {
                    std::for_each(it.second.begin(), it.second.end(), [](Event* e) { Tools::Delete(e); });
                });
    }

    void EventManager::DispatchEvents()
    {
        auto it = this->_events.begin();
        auto itEnd = this->_events.end();
        for (; it != itEnd; ++it)
        {
            if (it->first > this->_engine.GetCurrentTime())
                break;
            auto itEvent = it->second.begin();
            auto itEventEnd = it->second.end();
            for (; itEvent != itEventEnd; ++itEvent)
            {
                this->_engine.GetEntityManager().CallEntityFunction(
                        (*itEvent)->entityId,
                        (*itEvent)->function,
                        (*itEvent)->copiedArgs);
                Tools::Delete(*itEvent);
            }
        }
        this->_events.erase(this->_events.begin(), it);
    }

    void EventManager::_CallLater(Tools::Lua::CallHelper& helper)
    {
        double seconds = helper.PopArg().CheckNumber();
        if (seconds < 0)
            seconds = 0;
        int entityId = helper.PopArg().CheckInteger();
        std::string function = helper.PopArg().CheckString();
        Tools::Lua::Ref copiedArgs(this->_engine.GetInterpreter().GetState());
        if (helper.GetNbArgs() > 0)
            copiedArgs = this->_engine.GetInterpreter().GetSerializer().MakeSerializableCopy(helper.PopArg());
        Event* e = new Event(entityId, function, copiedArgs);
        this->_events[this->_engine.GetCurrentTime() + seconds * 1000000.0].push_back(e);
    }

}}}

