#include <luasel/Luasel.hpp>

#include "server/game/engine/CallbackManager.hpp"
#include "server/game/engine/Engine.hpp"
#include "server/game/engine/EntityManager.hpp"
#include "server/game/map/Map.hpp"
#include "tools/database/IConnection.hpp"

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

    Uint32 CallbackManager::MakeCallback(Uint32 entityId, std::string const& function, Luasel::Ref const& arg, bool serialize /* = true */)
    {
        while (!this->_nextCallbackId // 0 est la valeur spéciale "pas de callback", on la saute
                || this->_callbacks.count(this->_nextCallbackId))
            ++this->_nextCallbackId;
        this->_callbacks[this->_nextCallbackId] = new Callback(entityId, function,
                serialize ? this->_engine.GetInterpreter().GetSerializer().MakeSerializableCopy(arg, true) : arg);
        return this->_nextCallbackId++;
    }

    CallbackManager::Result CallbackManager::TriggerCallback(Uint32 callbackId, Luasel::Ref* ret /* = 0 */, bool keepCallback /* = false */)
    {
        return this->TriggerCallback(callbackId, this->_engine.GetInterpreter().MakeNil(), ret, keepCallback);
    }

    CallbackManager::Result CallbackManager::TriggerCallback(Uint32 callbackId, Luasel::Ref const& bonusArg, Luasel::Ref* ret /* = 0 */, bool keepCallback /* = false */)
    {
        if (!callbackId)
            return Ok;
        auto it = this->_callbacks.find(callbackId);
        if (it != this->_callbacks.end())
        {
            Result res = this->_engine.GetEntityManager().CallEntityFunction(it->second->entityId, it->second->function, it->second->arg, bonusArg, ret);
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

    CallbackManager::Callback const& CallbackManager::GetCallback(Uint32 callbackId) const throw(std::runtime_error)
    {
        auto it = this->_callbacks.find(callbackId);
        if (it == this->_callbacks.end())
            throw std::runtime_error("CallbackManager: Callback not found.");
        return *it->second;
    }

    void CallbackManager::Save(Tools::Database::IConnection& conn)
    {
        std::string table = this->_engine.GetMap().GetName() + "_callback";
        conn.CreateQuery("DELETE FROM " + table)->ExecuteNonSelect();
        auto query = conn.CreateQuery("INSERT INTO " + table + " (id, entity_id, function, arg) VALUES (?, ?, ?, ?);");
        auto it = this->_callbacks.begin();
        auto itEnd = this->_callbacks.end();
        for (; it != itEnd; ++it)
            try
            {
                std::string arg = this->_engine.GetInterpreter().GetSerializer().Serialize(it->second->arg, true /* nilOnError */);
                Tools::debug << ">> Save >> " << table << " >> Callback (id: " << it->first << ", entityId: " << it->second->entityId << ", function: \"" << it->second->function << "\", arg: " << arg.size() << " bytes)" << std::endl;
                query->Bind(it->first).Bind(it->second->entityId).Bind(it->second->function).Bind(arg).ExecuteNonSelect().Reset();
            }
            catch (std::exception& e)
            {
                Tools::error << "CallbackManager::Save: Could not save callback " << it->first << ": " << e.what() << std::endl;
            }
    }

    void CallbackManager::Load(Tools::Database::IConnection& conn)
    {
        std::string table = this->_engine.GetMap().GetName() + "_callback";
        auto query = conn.CreateQuery("SELECT id, entity_id, function, arg FROM " + table);
        while (auto row = query->Fetch())
        {
            Uint32 id = row->GetUint32(0);
            Uint32 entityId = row->GetUint32(1);
            std::string function = row->GetString(2);
            try
            {
                Luasel::Ref arg = this->_engine.GetInterpreter().GetSerializer().Deserialize(row->GetString(3));
                Tools::debug << "<< Load << " << table << " << Callback (id: " << id << ", entityId: " << entityId << ", function: \"" << function << "\", arg: <lua>)" << std::endl;
                this->_callbacks[id] = new Callback(entityId, function, arg);
            }
            catch (std::exception& e) // erreur de deserialization
            {
                Tools::error << "CallbackManager::Load: Could not load callback " << id << ": " << e.what() << std::endl;
            }
        }
    }

}}}
