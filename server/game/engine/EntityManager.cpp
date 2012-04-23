#include "server/precompiled.hpp"

#include "server/game/engine/EntityManager.hpp"
#include "server/game/engine/Engine.hpp"
#include "server/game/engine/Entity.hpp"
#include "server/game/engine/PositionalEntity.hpp"
#include "server/game/engine/EntityType.hpp"
#include "server/game/Game.hpp"
#include "server/game/World.hpp"
#include "server/game/PluginManager.hpp"
#include "tools/lua/Interpreter.hpp"
#include "tools/lua/MetaTable.hpp"
#include "tools/database/sqlite/Connection.hpp"
#include "server/game/map/Map.hpp"
#include "common/FieldValidator.hpp"
#include "server/rcon/ToJsonStr.hpp"
#include "server/rcon/EntityManager.hpp"
#include "tools/lua/utils/Vector.hpp"

namespace Server { namespace Game { namespace Engine {

    EntityManager::EntityManager(Engine& engine) :
        _engine(engine),
        _nextEntityId(1), // la première entité sera la 1, 0 est la valeur spéciale "pas d'entité"
        _runningEntityId(0),
        _runningEntity(0)
    {
        Tools::debug << "EntityManager::EntityManager()\n";
        auto& i = this->_engine.GetInterpreter();
        auto namespaceTable = i.Globals()["Server"].Set("Entity", i.MakeTable());
        namespaceTable.Set("Spawn", i.MakeFunction(std::bind(&EntityManager::_ApiSpawn, this, std::placeholders::_1)));
        namespaceTable.Set("SpawnFromPlugin", i.MakeFunction(std::bind(&EntityManager::_ApiSpawnFromPlugin, this, std::placeholders::_1)));
        namespaceTable.Set("Kill", i.MakeFunction(std::bind(&EntityManager::_ApiKill, this, std::placeholders::_1)));
        namespaceTable.Set("Register", i.MakeFunction(std::bind(&EntityManager::_ApiRegister, this, std::placeholders::_1)));
        namespaceTable.Set("RegisterPositional", i.MakeFunction(std::bind(&EntityManager::_ApiRegisterPositional, this, std::placeholders::_1)));
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
        assert(!this->_runningEntityId && !this->_engine.GetRunningEntityId() && "chaînage de calls Lua, THIS IS BAD");
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
            if (f.IsFunction())
            {
                this->_runningEntityId = targetId;
                this->_runningEntity = it->second;
                if (bonusArg.Exists())
                    f(it->second->GetSelf(), arg, bonusArg);
                else
                    f(it->second->GetSelf(), arg);
                this->_runningEntity = 0;
                this->_runningEntityId = 0;
            }
            else
            {
                Tools::error << "EntityManager::LuaFunctionCall: Call to \"" << function << "\" for entity " << targetId << " (\"" << it->second->GetType().GetName() << "\") failed: function not found (type: " << f.GetTypeName() << ").\n";
                return CallbackManager::FunctionNotFound;
            }
        }
        catch (std::exception& e)
        {
            Tools::error << "EntityManager::LuaFunctionCall: Fatal (entity deleted): Call to \"" << function << "\" for entity " << targetId << " (\"" << it->second->GetType().GetName() << "\") failed: " << e.what() << std::endl;
            this->_DeleteEntity(it->first, it->second);
            this->_runningEntity = 0;
            this->_runningEntityId = 0;
            return CallbackManager::Error;
        }
        Tools::debug << "EntityManager::LuaFunctionCall: Function \"" << function << "\" called for entity " << targetId << " (\"" << it->second->GetType().GetName() << "\").\n";
        return CallbackManager::Ok;
    }

    void EntityManager::AddSpawnEvent(Uint32 pluginId, std::string const& entityName, Tools::Lua::Ref const& arg, Uint32 spawnerId, Uint32 notificationCallbackId, Common::Position const& pos /* = Common::Position() */)
    {
        this->_spawnEvents.push(new SpawnEvent(pluginId, entityName, arg, spawnerId, notificationCallbackId, pos));
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
                this->_DeleteEntity(it->first, it->second);
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
                query->Bind(it->first).Bind(it->second->GetType().GetName()).Bind(this->_engine.GetInterpreter().GetSerializer().Serialize(it->second->GetSelf()["storage"])).ExecuteNonSelect().Reset();
            }
            catch (std::exception& e)
            {
                Tools::error << "EntityManager::Save: Could not save entity " << it->first << " (of type \"" << it->second->GetType().GetName() << "\"): " << e.what() << std::endl;
            }
        }
        conn.EndTransaction();
    }

    void EntityManager::BootstrapPlugin(Uint32 pluginId)
    {
        // TODO rajouter un "if !(le plugin a deja ete charge une fois dans cette map)"
        this->AddSpawnEvent(pluginId, "Init", this->_engine.GetInterpreter().MakeNil() /* arg */, 0 /* spawnerId */, 0 /* callbackId */);
    }

    Uint32 EntityManager::GetRunningPluginId() const
    {
        return this->_runningEntity ? this->_runningEntity->GetType().GetPluginId() : 0;
    }

    Uint32 EntityManager::_CreateEntity(Uint32 pluginId, std::string entityName, bool positional /* = false */, Common::Position const& pos /* = Common::Position() */) throw(std::runtime_error)
    {
        // trouve le plugin
        auto itPlugin = this->_entityTypes.find(pluginId);
        if (itPlugin == this->_entityTypes.end())
            throw std::runtime_error("unknown plugin " + Tools::ToString(pluginId));

        // trouve le prototype
        auto itType = itPlugin->second.find(entityName);
        if (itType == itPlugin->second.end())
            throw std::runtime_error("unknown entity \"" + entityName + "\" in plugin " + Tools::ToString(pluginId));

        // une position est necessaire si c'est une entité positionnelle
        if (itType->second->IsPositional() && !positional)
            throw std::runtime_error("no position given for creating a positional entity \"" + entityName + "\" in plugin " + Tools::ToString(pluginId));

        // trouve le prochain entityId
        while (!this->_nextEntityId // 0 est la valeur spéciale "pas d'entité", on la saute
                && this->_entities.count(this->_nextEntityId))
            ++this->_nextEntityId;

        // allocation
        Entity* entity;
        if (itType->second->IsPositional())
        {
            PositionalEntity* positionalEntity = new PositionalEntity(this->_engine, this->_nextEntityId, itType->second, pos);
            this->_positionalEntities[this->_nextEntityId] = positionalEntity;
            entity = positionalEntity;
        }
        else
            entity = new Entity(this->_engine, this->_nextEntityId, itType->second);
        this->_entities[this->_nextEntityId] = entity;

        Tools::debug << "EntityManager::_CreateEntity: New entity \"" << entityName << "\" (plugin " << pluginId << ") created (" << this->_nextEntityId << ").\n";
        return this->_nextEntityId++;
    }

    void EntityManager::_DeleteEntity(Uint32 id, Entity* entity)
    {
        if (entity->GetType().IsPositional())
        {
            size_t nbErased = this->_positionalEntities.erase(id);
            assert(nbErased == 1 && "j'ai voulu supprimer une entité positionnelle mais elle était pas dans sa map");
        }
        this->_entities.erase(id);
        Tools::Delete(entity);
    }

    void EntityManager::_ApiSpawnFromPlugin(Tools::Lua::CallHelper& helper)
    {
        Common::Position pos;
        Tools::Lua::Ref firstArg = helper.PopArg("Server.Entity.Spawn[FromPlugin]: Missing argument \"entityName\"");
        if (firstArg.IsTable())
        {
            pos = Tools::Lua::Utils::Vector::TableToVec3<double>(firstArg);
            firstArg = helper.PopArg("Server.Entity.Spawn[FromPlugin]: Missing argument \"entityName\"");
        }
        std::string plugin = firstArg.CheckString("Server.Entity.Spawn[FromPlugin]: Argument \"plugin\" must be a string");
        Uint32 pluginId = this->_engine.GetWorld().GetPluginManager().GetPluginId(plugin);
        this->_SpawnFromPlugin(pos, pluginId, helper);
    }

    void EntityManager::_ApiSpawn(Tools::Lua::CallHelper& helper)
    {
        Common::Position pos;
        if (helper.GetNbArgs() && helper.GetArgList().front().IsTable())
            pos = Tools::Lua::Utils::Vector::TableToVec3<double>(helper.PopArg());
        this->_SpawnFromPlugin(pos, this->_engine.GetRunningPluginId(), helper);
    }

    void EntityManager::_SpawnFromPlugin(Common::Position const& pos, Uint32 pluginId, Tools::Lua::CallHelper& helper)
    {
        std::string entityName = helper.PopArg("Server.Entity.Spawn[FromPlugin]: Missing argument \"entityName\"").CheckString("Server.Entity.Spawn[FromPlugin]: Argument \"entityName\" must be a string");
        Tools::Lua::Ref arg(this->_engine.GetInterpreter().GetState());
        Uint32 cbTargetId = 0;
        std::string cbFunction;
        Tools::Lua::Ref cbArg(this->_engine.GetInterpreter().GetState());
        if (helper.GetNbArgs())
        {
            arg = helper.PopArg();
            if (helper.GetNbArgs())
            {
                cbTargetId = static_cast<Uint32>(helper.PopArg().CheckNumber("Server.Entity.Spawn[FromPlugin]: Argument \"cbTarget\" must be a number"));
                cbFunction = helper.PopArg("Server.Entity.Spawn[FromPlugin]: Missing argument \"cbFunction\"").CheckString("Server.Entity.Spawn[FromPlugin]: Argument \"cbFunction\" must be a string");
                if (helper.GetNbArgs())
                    cbArg = helper.PopArg();
            }
        }
        Uint32 callbackId = 0;
        if (cbTargetId)
            callbackId = this->_engine.GetCallbackManager().MakeCallback(cbTargetId, cbFunction, cbArg);
        this->AddSpawnEvent(pluginId, entityName, arg, this->_engine.GetRunningEntityId(), callbackId, pos);
    }

    void EntityManager::_ApiKill(Tools::Lua::CallHelper& helper)
    {
        Uint32 targetId = helper.PopArg("Server.Entity.Kill: Missing argument \"target\"").Check<Uint32>("Server.Entity.Kill: Argument \"target\" must be a number");
        Tools::Lua::Ref arg(this->_engine.GetInterpreter().GetState());
        Uint32 cbTargetId = 0;
        std::string cbFunction;
        Tools::Lua::Ref cbArg(this->_engine.GetInterpreter().GetState());
        if (helper.GetNbArgs())
        {
            arg = helper.PopArg();
            if (helper.GetNbArgs())
            {
                cbTargetId = static_cast<Uint32>(helper.PopArg().CheckNumber("Server.Entity.Kill: Argument \"cbTarget\" must be a number"));
                cbFunction = helper.PopArg("Server.Entity.Kill: Missing argument \"cbFunction\"").CheckString("Server.Entity.Kill: Argument \"cbFunction\" must be a string");
                if (helper.GetNbArgs())
                    cbArg = helper.PopArg();
            }
        }
        Uint32 callbackId = 0;
        if (cbTargetId)
            callbackId = this->_engine.GetCallbackManager().MakeCallback(cbTargetId, cbFunction, cbArg);
        this->AddKillEvent(targetId, arg, this->_engine.GetRunningEntityId(), callbackId);
    }

    void EntityManager::_ApiRegister(Tools::Lua::CallHelper& helper)
    {
        Uint32 pluginId = this->_engine.GetRunningPluginId();
        if (!pluginId)
            throw std::runtime_error("Server.Entity.Register[Positional]: Could not determine currently running plugin, aborting registration.");
        std::string pluginName = this->_engine.GetWorld().GetPluginManager().GetPluginIdentifier(pluginId);
        Tools::Lua::Ref prototype(this->_engine.GetInterpreter().GetState());
        std::string entityName;
        bool positional = false;
        try
        {
            prototype = helper.PopArg();
            if (!prototype.IsTable())
                throw std::runtime_error("Server.Entity.Register[Positional]: Argument \"prototype\" must be of type table (instead of " + prototype.GetTypeName() + ")");
            if (!prototype["entityName"].IsString())
                throw std::runtime_error("Server.Entity.Register[Positional]: Field \"entityName\" in prototype must exist and be of type string");
            if (!Common::FieldValidator::IsEntityType(entityName = prototype["entityName"].ToString()))
                throw std::runtime_error("Server.Entity.Register[Positional]: Invalid entity name");
            if (helper.GetNbArgs() && helper.PopArg().ToBoolean()) // bool flag en deuxieme parametre pour indiquer que c'est RegisterPositional()
                positional = true;
        }
        catch (std::exception& e)
        {
            Tools::error << "EntityManager::_ApiRegister: Failed to register new entity type from \"" << pluginName << "\": " << e.what() << " (plugin " << pluginId << ").\n";
            return;
        }
        if (this->_entityTypes[pluginId].count(entityName)) // remplacement
        {
            EntityType* type = this->_entityTypes[pluginId][entityName];
            if (type->IsPositional() != positional)
                throw std::runtime_error("Server.Entity.Register[Positional]: Cannot change the positional property of an entity type.");
            type->SetPrototype(prototype);
            Tools::log << "EntityManager::_ApiRegister: Replacing entity type \"" << entityName << "\" with a newer type from \"" << pluginName << "\" (plugin " << pluginId << ", positional: " << (positional ? "yes" : "no") << ").\n";
        }
        else // nouveau type
        {
            this->_entityTypes[pluginId][entityName] = new EntityType(entityName, pluginId, prototype, positional);
            Tools::debug << "EntityManager::_ApiRegister: New entity type \"" << entityName << "\" registered from \"" << pluginName << "\" (plugin " << pluginId << ", positional: " << (positional ? "yes" : "no") << ").\n";
        }
    }

    void EntityManager::_ApiRegisterPositional(Tools::Lua::CallHelper& helper)
    {
        helper.GetArgList().push_back(this->_engine.GetInterpreter().MakeBoolean(true));
        this->_ApiRegister(helper);
    }

    std::string EntityManager::RconGetEntities() const
    {
        std::string json = "[\n";
        auto it = this->_entities.begin();
        auto itEnd = this->_entities.end();
        for (; it != itEnd; ++it)
        {
            if (it != this->_entities.begin())
                json += ",\n";
            json +=
                "\t{\n"
                "\t\t\"id\": " + Tools::ToString(it->first) + ",\n" +
                "\t\t\"type\": \"" + it->second->GetType().GetName() + "\",\n" +
                "\t\t\"plugin\": \"" + this->_engine.GetWorld().GetPluginManager().GetPluginIdentifier(it->second->GetType().GetPluginId()) + "\",\n" +
                "\t\t\"positional\": " + (it->second->GetType().IsPositional() ? "true" : "false") + "\n" +
                "\t}";
        }
        json += "\n]\n";
        return json;
    }

    void EntityManager::RconAddEntityTypes(Rcon::EntityManager& manager) const
    {
        auto it = this->_entityTypes.begin();
        auto itEnd = this->_entityTypes.end();
        for (; it != itEnd; ++it)
        {
            auto it2 = it->second.begin();
            auto itEnd2 = it->second.end();
            for (; it2 != itEnd2; ++it2)
                manager.AddType(it2->second->GetPluginId(), it2->second->GetName(), it2->second->IsPositional());
        }
    }

}}}
