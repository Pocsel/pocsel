#include "server/precompiled.hpp"

#include "server/game/engine/EntityManager.hpp"
#include "server/game/engine/EntityStorage.hpp"
#include "server/game/engine/Engine.hpp"
#include "tools/lua/Interpreter.hpp"
#include "tools/lua/MetaTable.hpp"
#include "tools/database/sqlite/Connection.hpp"
#include "server/game/map/Map.hpp"

namespace Server { namespace Game { namespace Engine {

    EntityManager::EntityManager(Engine& engine) :
        _engine(engine), _pluginIdForRegistering(0), _currentEntityId(0), _lastCalledEntityId(0)
    {
        Tools::debug << "EntityManager::EntityManager()\n";
        this->_engine.GetInterpreter().Globals()["Server"].Set("Entity", this->_engine.GetInterpreter().MakeTable());
    }

    EntityManager::~EntityManager()
    {
        Tools::debug << "EntityManager::~EntityManager()\n";
        std::for_each(this->_entities.begin(), this->_entities.end(), [](std::pair<int const, Entity*>& it) { Tools::Delete(it.second); });
        auto itPlugin = this->_entityTypes.begin();
        auto itPluginEnd = this->_entityTypes.end();
        for (; itPlugin != itPluginEnd; ++itPlugin)
        {
            auto itType = itPlugin->second.begin();
            auto itTypeEnd = itPlugin->second.end();
            for (; itType != itTypeEnd; ++itType)
                Tools::Delete(itType->second);
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
                query->Bind(it->first).Bind(it->second->type->name).Bind(this->_engine.GetInterpreter().GetSerializer().Serialize(it->second->self["storage"])).ExecuteNonSelect().Reset();
            }
            catch (std::exception& e)
            {
                Tools::error << "EntityManager::Save: Could not save entity " << it->first << " (of type \"" << it->second->type->entityName << "\"): " << e.what() << std::endl;
            }
        }
        conn.EndTransaction();
    }

    void EntityManager::BeginPluginRegistering(Uint32 pluginId)
    {
        this->_pluginIdForRegistering = pluginId;
        this->_engine.GetInterpreter().Globals()["Server"]["Entity"].Set("Register",
                this->_engine.GetInterpreter().MakeFunction(std::bind(&EntityManager::_ApiRegister, this, std::placeholders::_1)));
    }

    void EntityManager::EndPluginRegistering()
    {
        this->_pluginIdForRegistering = 0;
        this->_engine.GetInterpreter().Globals()["Server"]["Entity"].Set("Register",
                this->_engine.GetInterpreter().MakeNil());
    }

    int EntityManager::SpawnEntity(std::string name, Uint32 pluginId, Uint32 callbackId) throw(std::runtime_error)
    {
        // trouve le plugin
        auto itPlugin = this->_entityTypes.find(pluginId);
        if (itPlugin == this->_entityTypes.end())
            throw std::runtime_error("EntityManager::SpawnEntity: Unknown plugin " + Tools::ToString(pluginId));

        // trouve le prototype
        auto itType = itPlugin->second.find(name);
        if (itType == itPlugin->second.end())
            throw std::runtime_error("EntityManager::SpawnEntity: Unknown entity \"" + name + "\" in plugin " + Tools::ToString(pluginId));

        // trouve le prochain entityId
        while (this->_entities.count(this->_currentEntityId))
            ++this->_currentEntityId;

        // allocation
        Entity* entity = new Entity(itType->second, this->_engine.GetInterpreter().MakeTable());
        this->_entities[this->_currentEntityId] = entity;

        // metatable pour le prototype
        Tools::Lua::Ref metatable = this->_engine.GetInterpreter().MakeTable();
        metatable.Set("__index", itType->second->prototype);
        entity->self.SetMetaTable(metatable);

        Tools::debug << "EntityManager::SpawnEntity: New entity \"" << name << "\" (plugin " << pluginId << ") spawned.\n";
        this->CallEntityFunction(this->_currentEntityId, "Spawn", args);
        return this->_currentEntityId++;
    }

    void EntityManager::BootstrapPlugin(Uint32 pluginId)
    {
        // TODO rajouter un "if !(le plugin a deja ete charge une fois dans cette map)"
        try
        {
            this->SpawnEntity("Init", pluginId, this->_engine.GetInterpreter().MakeNil());
        }
        catch (std::exception& e)
        {
            Tools::log << "EntityManager::StartPlugin: Error: " << e.what() << std::endl;
        }
    }

    bool EntityManager::TriggerCallback(CallbackManager::Callback const& callback)
    {
        auto it = this->_entities.find(callback.targetId);
        if (it == this->_entities.end())
        {
            Tools::error << "EntityManager::TriggerCallback: Call to \"" << callback.function << "\" for entity " << callback.targetId << " failed: entity not found.\n";
            return false;
        }
        try
        {
            this->_lastCalledEntityId = entityId;
            //it->second->self.Set("id", entityId);
            if (callback.bonusArg.Exists())
                it->second->type->prototype[callback.function](it->second->self, args, bonusArg);
            else
                it->second->type->prototype[callback.function](it->second->self, args);
        }
        catch (std::exception& e)
        {
            Tools::error << "EntityManager::TriggerCallback: Call to \"" << callback.function << "\" for entity " << callback.targetId << " failed: " << e.what() << std::endl;
        }
        Tools::debug << "EntityManager::TriggerCallback: Function \"" << callback.function << "\" called for entity " << callback.targetId << ".\n";
        return true;
    }

    int EntityManager::GetLastCalledEntityId() const
    {
        return this->_lastCalledEntityId;
    }

    void EntityManager::_ApiSpawn(Tools::Lua::CallHelper& helper)
    {
        // TODO paps
    }

    void EntityManager::_ApiRegister(Tools::Lua::CallHelper& helper)
    {
        assert(this->_pluginIdForRegistering != 0 && "_ApiRegister ne doit pas etre accessible en dehors de la phase d'enregistrement des entites");
        Tools::Lua::Ref prototype(this->_engine.GetInterpreter().GetState());
        std::string entityName;
        try
        {
            prototype = helper.PopArg();
            if (!prototype.IsTable())
                throw std::runtime_error("expected one parameter of type table");
            if (!Common::FieldValidator::IsEntityType(itentityName = prototype["entityName"].CheckString()))
                throw std::runtime_error("invalid entity name");
        }
        catch (std::exception& e)
        {
            Tools::error << "EntityManager::_ApiRegister: " << e.what() << std::endl;
            return;
        }
        EntityType* type = new EntityType(entityName, this->_pluginIdForRegistering, prototype);
        this->_entityTypes[this->_pluginIdForRegistering][type->entityName] = type;
        Tools::debug << "EntityManager::_ApiRegister: New entity type \"" << type->entityName << "\" registered (plugin " << type->pluginId << ").\n";
    }

}}}
