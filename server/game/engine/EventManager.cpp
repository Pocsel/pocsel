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
        std::for_each(this->_events.begin(), this->_events.end(), [](std::pair<Uint64 const, Event*>& it)
                {
                    Tools::Delete(it.second);
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
            this->_engine.GetEntityManager().CallEntityFunction(
                    it->second->entityId,
                    it->second->function,
                    this->_engine.GetInterpreter().Deserialize(it->second->args));
            Tools::Delete(it->second);
        }
        this->_events.erase(this->_events.begin(), it);
    }

    void EventManager::_CallLater(Tools::Lua::CallHelper& helper)
    {
        double seconds = helper.PopArg().CheckNumber();
        int entityId = helper.PopArg().CheckInteger();
        std::string function = helper.PopArg().CheckString();
        std::string args;
        if (helper.GetNbArgs() > 0)
            args = this->_engine.GetInterpreter().Serialize(helper.PopArg());
        Event* e = new Event;
        e->entityId = entityId;
        e->function = function;
        e->args = args;
        this->_events[this->_engine.GetCurrentTime() + seconds / 1000000] = e;
    }

}}}

