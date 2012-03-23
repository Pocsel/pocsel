#include "server/precompiled.hpp"

#include "server/game/engine/EntityManager.hpp"
#include "server/game/engine/Engine.hpp"
#include "tools/lua/Interpreter.hpp"
#include "tools/lua/MetaTable.hpp"
#include "tools/database/sqlite/Connection.hpp"
#include "server/game/map/Map.hpp"
#include "common/FieldValidator.hpp"

namespace Server { namespace Game { namespace Engine {

    EntityManager::EntityManager(Engine& engine) :
        _engine(engine),
        _pluginIdForRegistering(0),
        _nextEntityId(1), // la première entité sera la 1, 0 est la valeur spéciale "pas d'entité"
        _runningEntityId(0)
    {
        Tools::debug << "EntityManager::EntityManager()\n";
        auto& i = this->_engine.GetInterpreter();
        auto namespaceTable = i.Globals()["Server"].Set("Entity", i.MakeTable());
        namespaceTable.Set("Spawn", i.MakeFunction(std::bind(&EntityManager::_ApiSpawn, this, std::placeholders::_1)));
        namespaceTable.Set("SpawnFromPlugin", i.MakeFunction(std::bind(&EntityManager::_ApiSpawnFromPlugin, this, std::placeholders::_1)));
        namespaceTable.Set("Kill", i.MakeFunction(std::bind(&EntityManager::_ApiKill, this, std::placeholders::_1)));
    }

    EntityManager::~EntityManager()
    {
        Tools::debug << "EntityManager::~EntityManager()\n";
        // entities
        auto itEntity = this->_entities.begin();
        auto itEntityEnd = this->_entities.end();
        for (; itEntity != itEntityEnd; ++itEntity)
            Tools::Delete(itEntity->second);
        // entity types
        auto itPlugin = this->_entityTypes.begin();
        auto itPluginEnd = this->_entityTypes.end();
        for (; itPlugin != itPluginEnd; ++itPlugin)
        {
            auto itType = itPlugin->second.begin();
            auto itTypeEnd = itPlugin->second.end();
            for (; itType != itTypeEnd; ++itType)
                Tools::Delete(itType->second);
        }
        // spawn events
        while (!this->_spawnEvents.empty())
        {
            Tools::Delete(this->_spawnEvents.front());
            this->_spawnEvents.pop();
        }
        // kill events
        while (!this->_killEvents.empty())
        {
            Tools::Delete(this->_killEvents.front());
            this->_killEvents.pop();
        }
    }

    CallbackManager::Result EntityManager::LuaFunctionCall(Uint32 targetId, std::string const& function, Tools::Lua::Ref const& arg, Tools::Lua::Ref const& bonusArg)
    {
        assert(!this->_runningEntityId && "chaînage de calls Lua, THIS IS BAD");
        assert(!this->_pluginIdForRegistering && "HOLY SHIT, call Lua durant la phase d'enregistrement des entités");
        auto it = this->_entities.find(targetId);
        if (it == this->_entities.end())
        {
            Tools::error << "EntityManager::LuaFunctionCall: Call to \"" << function << "\" for entity " << targetId << " failed: entity not found.\n";
            return CallbackManager::EntityNotFound;
        }
        try
        {
            auto prototype = it->second->GetType().GetPrototype();
            if (!prototype.IsTable())
                throw std::runtime_error("prototype is not a table");
            auto f = it->second->GetType().GetPrototype()[function];
        }
        catch (std::exception& e)
        {
            Tools::error << "EntityManager::LuaFunctionCall: Fatal (entity deleted): Call to \"" << function << "\" for entity " << targetId << " (\"" << it->second->type->entityName << "\") failed: " << e.what() << std::endl;
            delete it->second;
            this->_entities.erase(it);
            this->_runningEntityId = 0;
            return CallbackManager::Error;
        }
        Tools::debug << "EntityManager::LuaFunctionCall: Function \"" << function << "\" called for entity " << targetId << " (\"" << it->second->GetType().GetName() << "\").\n";
        return CallbackManager::Ok;


        auto prototype = it->second->GetType().GetPrototype();
        auto f = it->second->GetType().GetPrototype()[function];
        if (f.IsFunction())
        {
            this->_runningEntityId = targetId;
            try
            {
                if (bonusArg.Exists())
                    f(it->second->self, arg, bonusArg);
                else
                    f(it->second->self, arg);
            }
            catch (std::exception& e)
            {
                Tools::error << "EntityManager::LuaFunctionCall: Fatal (entity deleted): Call to \"" << function << "\" for entity " << targetId << " (\"" << it->second->type->entityName << "\") failed: " << e.what() << std::endl;
                delete it->second;
                this->_entities.erase(it);
                this->_runningEntityId = 0;
                return CallbackManager::Error;
            }
            this->_runningEntityId = 0;
        }
        else
        {
            Tools::error << "EntityManager::LuaFunctionCall: Call to \"" << function << "\" for entity " << targetId << " (\"" << it->second->type->entityName << "\") failed: function not found.\n";
            return CallbackManager::FunctionNotFound;
        }
        Tools::debug << "EntityManager::LuaFunctionCall: Function \"" << function << "\" called for entity " << targetId << " (\"" << it->second->type->entityName << "\").\n";
        return CallbackManager::Ok;
    }

    Uint32 EntityManager::GetRunningEntityId() const
    {
        return this->_runningEntityId;
    }

    void EntityManager::AddSpawnEvent(Uint32 pluginId, std::string const& entityName, Tools::Lua::Ref const& arg, Uint32 spawnerId, Uint32 notificationCallbackId)
    {
        this->_spawnEvents.push(new SpawnEvent(pluginId, entityName, arg, spawnerId, notificationCallbackId));
    }

    void EntityManager::AddKillEvent(Uint32 targetId, Tools::Lua::Ref const& arg, Uint32 killerId, Uint32 notificationCallbackId)
    {
        this->_killEvents.push(new KillEvent(targetId, arg, killerId, notificationCallbackId));
    }

    void EntityManager::DispatchSpawnEvents()
    {
        while (!this->_spawnEvents.empty())
        {
            SpawnEvent* e = this->_spawnEvents.front();
            try
            {
                Uint32 newId = this->_CreateEntity(e->pluginId, e->entityName);
                CallbackManager::Result res = this->LuaFunctionCall(newId, "Spawn", e->arg, this->_engine.GetInterpreter().MakeNumber(e->spawnerId));
                if (res == CallbackManager::FunctionNotFound || res == CallbackManager::Ok)
                    this->_engine.GetCallbackManager().TriggerCallback(e->notificationCallbackId, this->_engine.GetInterpreter().MakeNumber(newId));
                else
                    this->_engine.GetCallbackManager().TriggerCallback(e->notificationCallbackId, this->_engine.GetInterpreter().MakeNumber(0));
            }
            catch (std::exception& ex)
            {
                Tools::error << "EntityManager::DispatchSpawnEvents: Cannot create entity \"" << e->entityName << "\" from plugin " << e->pluginId << ": " << ex.what() << std::endl;
                this->_engine.GetCallbackManager().TriggerCallback(e->notificationCallbackId, this->_engine.GetInterpreter().MakeNumber(0));
            }
            delete e;
            this->_spawnEvents.pop();
        }
    }

    void EntityManager::DispatchKillEvents()
    {
        while (!this->_killEvents.empty())
        {
            KillEvent* e = this->_killEvents.front();
            auto it = this->_entities.find(e->targetId);
            auto resultTable = this->_engine.GetInterpreter().MakeTable();
            resultTable.Set("entityId", this->_engine.GetInterpreter().MakeNumber(e->targetId));
            if (it != this->_entities.end())
            {
                this->LuaFunctionCall(e->targetId, "Die", e->arg, this->_engine.GetInterpreter().MakeNumber(e->killerId));
                delete it->second;
                this->_entities.erase(it);
                resultTable.Set("success", this->_engine.GetInterpreter().MakeBoolean(true)); // meme si le call a fail, l'entité est tuée quand même alors on retourne true
                this->_engine.GetCallbackManager().TriggerCallback(e->notificationCallbackId, resultTable);
            }
            else
            {
                Tools::error << "EntityManager::DispatchKillEvents: Cannot kill entity " << e->targetId << ": entity not found.\n";
                resultTable.Set("success", this->_engine.GetInterpreter().MakeBoolean(false));
                this->_engine.GetCallbackManager().TriggerCallback(e->notificationCallbackId, resultTable);
            }
            delete e;
            this->_killEvents.pop();
        }
    }

    void EntityManager::Save(Tools::Database::IConnection& conn)
    {
        conn.CreateQuery("DELETE FROM " + this->_engine.GetMap().GetName() + "_entity")->ExecuteNonSelect();
        conn.BeginTransaction();
        auto query = conn.CreateQuery("INSERT INTO " + this->_engine.GetMap().GetName() + "_entity (id, type, storage) VALUES (?, ?, ?)");
        auto it = this->_entities.begin();
        auto itEnd = this->_entities.end();
        for (; it != itEnd; ++it)
        {
            try
            {
                query->Bind(it->first).Bind(it->second->type->entityName).Bind(this->_engine.GetInterpreter().GetSerializer().Serialize(it->second->self["storage"])).ExecuteNonSelect().Reset();
            }
            catch (std::exception& e)
            {
                Tools::error << "EntityManager::Save: Could not save entity " << it->first << " (of type \"" << it->second->type->entityName << "\"): " << e.what() << std::endl;
            }
        }
        conn.EndTransaction();
    }

    void EntityManager::BeginPluginRegistering(Uint32 pluginId, std::string const& pluginName)
    {
        this->_pluginNameForRegistering = pluginName;
        this->_pluginIdForRegistering = pluginId;
        this->_engine.GetInterpreter().Globals()["Server"]["Entity"].Set("Register",
                this->_engine.GetInterpreter().MakeFunction(std::bind(&EntityManager::_ApiRegister, this, std::placeholders::_1)));
    }

    void EntityManager::EndPluginRegistering()
    {
        this->_pluginNameForRegistering.clear();
        this->_pluginIdForRegistering = 0;
        this->_engine.GetInterpreter().Globals()["Server"]["Entity"].Set("Register",
                this->_engine.GetInterpreter().MakeNil());
    }

    void EntityManager::BootstrapPlugin(Uint32 pluginId)
    {
        // TODO rajouter un "if !(le plugin a deja ete charge une fois dans cette map)"
        this->AddSpawnEvent(pluginId, "Init", this->_engine.GetInterpreter().MakeNil() /* arg */, 0 /* spawnerId */, 0 /* callbackId */);
    }

    Uint32 EntityManager::_CreateEntity(Uint32 pluginId, std::string entityName) throw(std::runtime_error)
    {
        // trouve le plugin
        auto itPlugin = this->_entityTypes.find(pluginId);
        if (itPlugin == this->_entityTypes.end())
            throw std::runtime_error("unknown plugin " + Tools::ToString(pluginId));

        // trouve le prototype
        auto itType = itPlugin->second.find(entityName);
        if (itType == itPlugin->second.end())
            throw std::runtime_error("unknown entity \"" + entityName + "\" in plugin " + Tools::ToString(pluginId));

        // trouve le prochain entityId
        while (!this->_nextEntityId // 0 est la valeur spéciale "pas d'entité", on la saute
                && this->_entities.count(this->_nextEntityId))
            ++this->_nextEntityId;

        // allocation
        Entity* entity = new Entity(itType->second, this->_engine.GetInterpreter().MakeTable());
        this->_entities[this->_nextEntityId] = entity;

        // configurationn du self
        // metatable pour le prototype
        Tools::Lua::Ref metatable = this->_engine.GetInterpreter().MakeTable();
        metatable.Set("__index", itType->second->prototype);
        entity->self.SetMetaTable(metatable);
        // champs par défaut
        entity->self.Set("id", this->_nextEntityId);
        entity->self.Set("prototype", itType->second->prototype);

        Tools::debug << "EntityManager::_CreateEntity: New entity \"" << entityName << "\" (plugin " << pluginId << ") created (" << this->_nextEntityId << ").\n";
        return this->_nextEntityId++;
    }

    void EntityManager::_ApiSpawnFromPlugin(Tools::Lua::CallHelper& helper)
    {
        // TODO paps
    }

    void EntityManager::_ApiSpawn(Tools::Lua::CallHelper& helper)
    {
        // TODO paps
    }

    void EntityManager::_ApiKill(Tools::Lua::CallHelper& helper)
    {
        // TODO paps
    }

    void EntityManager::_ApiRegister(Tools::Lua::CallHelper& helper)
    {
        assert(this->_pluginIdForRegistering && "_ApiRegister ne doit pas être accessible en dehors de la phase d'enregistrement des entités");
        Tools::Lua::Ref prototype(this->_engine.GetInterpreter().GetState());
        std::string entityName;
        try
        {
            prototype = helper.PopArg();
            if (!prototype.IsTable())
                throw std::runtime_error("expected one parameter of type table");
            if (!prototype["entityName"].IsString())
                throw std::runtime_error("\"entityName\" is of type " + prototype["entityName"].GetTypeName());
            if (!Common::FieldValidator::IsEntityType(entityName = prototype["entityName"].ToString()))
                throw std::runtime_error("invalid entity name");
        }
        catch (std::exception& e)
        {
            Tools::error << "EntityManager::_ApiRegister: Failed to register new entity type from \"" << this->_pluginNameForRegistering << "\": " << e.what() << " (plugin " << this->_pluginIdForRegistering << ").\n";
            return;
        }
        EntityType* type = new EntityType(entityName, this->_pluginIdForRegistering, prototype);
        if (this->_entityTypes[type->pluginId].count(type->entityName))
        {
            delete this->_entityTypes[type->pluginId][type->entityName];
            Tools::log << "EntityManager::_ApiRegister: Replacing entity type \"" << type->entityName << "\" with a newer type from \"" << this->_pluginNameForRegistering << "\" (plugin " << type->pluginId << ").\n";
        }
        this->_entityTypes[type->pluginId][type->entityName] = type;
        Tools::debug << "EntityManager::_ApiRegister: New entity type \"" << type->entityName << "\" registered from \"" << this->_pluginNameForRegistering << "\" (plugin " << type->pluginId << ").\n";
    }

}}}
