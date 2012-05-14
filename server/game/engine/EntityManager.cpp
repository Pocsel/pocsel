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
        auto itSpawn = this->_spawnEvents.begin();
        auto itSpawnEnd = this->_spawnEvents.end();
        for (; itSpawn != itSpawnEnd; ++itSpawn)
            Tools::Delete(*itSpawn);
        // kill events
        auto itKill = this->_killEvents.begin();
        auto itKillEnd = this->_killEvents.end();
        for (; itKill != itKillEnd; ++itKill)
            Tools::Delete(*itKill);
    }

    CallbackManager::Result EntityManager::CallEntityFunction(Uint32 targetId, std::string const& function, Tools::Lua::Ref const& arg, Tools::Lua::Ref const& bonusArg, Tools::Lua::Ref* ret /* = 0 */)
    {
        assert(!this->_runningEntityId && !this->_engine.GetRunningEntityId() && "chaînage de calls Lua, THIS IS BAD");
        auto it = this->_entities.find(targetId);
        if (it == this->_entities.end() || !it->second)
        {
            Tools::error << "EntityManager::CallEntityFunction: Call to \"" << function << "\" for entity " << targetId << " failed: entity not found.\n";
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
                    ret ? *ret = f(it->second->GetSelf(), arg, bonusArg) : f(it->second->GetSelf(), arg, bonusArg);
                else
                    ret ? *ret = f(it->second->GetSelf(), arg) : f(it->second->GetSelf(), arg);
                this->_runningEntity = 0;
                this->_runningEntityId = 0;
            }
            else
            {
                Tools::error << "EntityManager::CallEntityFunction: Call to \"" << function << "\" for entity " << targetId << " (\"" << it->second->GetType().GetName() << "\") failed: function not found (type: " << f.GetTypeName() << ").\n";
                return CallbackManager::FunctionNotFound;
            }
        }
        catch (std::exception& e)
        {
            Tools::error << "EntityManager::CallEntityFunction: Fatal (entity deleted): Call to \"" << function << "\" for entity " << targetId << " (\"" << it->second->GetType().GetName() << "\") failed: " << e.what() << std::endl;
            this->_DeleteEntity(it->first, it->second);
            this->_runningEntity = 0;
            this->_runningEntityId = 0;
            return CallbackManager::Error;
        }
        //Tools::debug << "EntityManager::CallEntityFunction: Function \"" << function << "\" called for entity " << targetId << " (\"" << it->second->GetType().GetName() << "\").\n";
        return CallbackManager::Ok;
    }

    void EntityManager::AddSpawnEvent(Uint32 pluginId, std::string const& entityName, Tools::Lua::Ref const& arg, Uint32 spawnerId, Uint32 notificationCallbackId, Common::Position const& pos /* = Common::Position() */)
    {
        this->_spawnEvents.push_back(new SpawnEvent(pluginId, entityName, arg, spawnerId, notificationCallbackId, pos));
    }

    void EntityManager::AddKillEvent(Uint32 targetId, Tools::Lua::Ref const& arg, Uint32 killerId, Uint32 notificationCallbackId)
    {
        this->_killEvents.push_back(new KillEvent(targetId, arg, killerId, notificationCallbackId));
    }

    void EntityManager::DispatchSpawnEvents()
    {
        // buffer pour les callbacks de notification de spawn
        // evite de niquer le parcours de _spawnEvents quand un spawn est demandé dans une callback de notification
        std::queue<std::pair<Uint32 /* notificationCallbackId */, Tools::Lua::Ref /* resultTable */>> notifications;

        auto it = this->_spawnEvents.begin();
        auto itEnd = this->_spawnEvents.end();
        for (; it != itEnd; ++it)
        {
            SpawnEvent* e = *it;
            auto resultTable = this->_engine.GetInterpreter().MakeTable();
            try
            {
                // trouve le prochain entityId
                while (!this->_nextEntityId // 0 est la valeur spéciale "pas d'entité", on la saute
                        && this->_entities.count(this->_nextEntityId))
                    ++this->_nextEntityId;
                Uint32 newId = this->_nextEntityId++;

                // XXX Spawn() hook
                Tools::Lua::Ref ret(this->_engine.GetInterpreter().GetState());
                this->_CreateEntity(newId, e->pluginId, e->entityName);
                if (this->CallEntityFunction(newId, "Spawn", e->arg, this->_engine.GetInterpreter().MakeNumber(e->spawnerId), &ret) == CallbackManager::Ok)
                    resultTable.Set("ret", this->_engine.GetInterpreter().GetSerializer().MakeSerializableCopy(ret, true));

                resultTable.Set("entityId", this->_engine.GetInterpreter().MakeNumber(newId));
                resultTable.Set("success", this->_engine.GetInterpreter().MakeBoolean(true));
                notifications.push(std::make_pair(e->notificationCallbackId, resultTable));
            }
            catch (std::exception& ex)
            {
                Tools::error << "EntityManager::DispatchSpawnEvents: Cannot create entity \"" << e->entityName << "\" from plugin " << e->pluginId << ": " << ex.what() << std::endl;
                resultTable.Set("success", this->_engine.GetInterpreter().MakeBoolean(false));
                notifications.push(std::make_pair(e->notificationCallbackId, resultTable));
            }
            delete e;
        }
        this->_spawnEvents.clear();

        while (!notifications.empty())
        {
            auto const& notif = notifications.front();
            this->_engine.GetCallbackManager().TriggerCallback(notif.first, notif.second);
            notifications.pop();
        }
    }

    void EntityManager::DispatchKillEvents()
    {
        // buffer pour les callbacks de notification de kill
        // evite de niquer le parcours de _killEvents quand un kill est demandé dans une callback de notification
        std::queue<std::pair<Uint32 /* notificationCallbackId */, Tools::Lua::Ref /* resultTable */>> notifications;

        auto it = this->_killEvents.begin();
        auto itEnd = this->_killEvents.end();
        for (; it != itEnd; ++it)
        {
            KillEvent* e = *it;
            auto it = this->_entities.find(e->targetId);
            auto resultTable = this->_engine.GetInterpreter().MakeTable();
            resultTable.Set("entityId", this->_engine.GetInterpreter().MakeNumber(e->targetId));
            if (it != this->_entities.end() && it->second)
            {
                Tools::Lua::Ref ret(this->_engine.GetInterpreter().GetState());

                // XXX Die() hook
                if (this->CallEntityFunction(e->targetId, "Die", e->arg, this->_engine.GetInterpreter().MakeNumber(e->killerId), &ret) == CallbackManager::Ok)
                    resultTable.Set("ret", this->_engine.GetInterpreter().GetSerializer().MakeSerializableCopy(ret, true));
                this->_DeleteEntity(it->first, it->second);
                resultTable.Set("success", this->_engine.GetInterpreter().MakeBoolean(true)); // meme si le call a fail, l'entité est tuée quand même alors on retourne true
                notifications.push(std::make_pair(e->notificationCallbackId, resultTable));
            }
            else
            {
                Tools::error << "EntityManager::DispatchKillEvents: Cannot kill entity " << e->targetId << ": entity not found.\n";
                resultTable.Set("success", this->_engine.GetInterpreter().MakeBoolean(false));
                notifications.push(std::make_pair(e->notificationCallbackId, resultTable));
            }
            delete e;
        }
        this->_killEvents.clear();

        while (!notifications.empty())
        {
            auto const& notif = notifications.front();
            this->_engine.GetCallbackManager().TriggerCallback(notif.first, notif.second);
            notifications.pop();
        }
    }

    void EntityManager::Save(Tools::Database::IConnection& conn)
    {
        // entities
        {
            std::string table = this->_engine.GetMap().GetName() + "_entity";
            conn.CreateQuery("DELETE FROM " + table)->ExecuteNonSelect();
            auto query = conn.CreateQuery("INSERT INTO " + table + " (id, plugin_id, entity_name, disabled, storage, pos_x, pos_y, pos_z) VALUES (?, ?, ?, ?, ?, ?, ?, ?);");
            // non positional entities
            {
                auto it = this->_entities.begin();
                auto itEnd = this->_entities.end();
                for (; it != itEnd; ++it)
                    if (it->second)
                    {
                        Entity* entity = it->second;
                        auto const& type = entity->GetType();
                        if (!type.IsPositional()) // saute les entités positionnelles, elles sont gérées differement après
                            try
                            {
                                // XXX Save() database hook
                                // (peut delete l'entité, mais les iterateurs restent valident pour la map)
                                CallbackManager::Result callRet = this->CallEntityFunction(it->first, "Save", this->_engine.GetInterpreter().MakeNil(), this->_engine.GetInterpreter().MakeNil());
                                if (callRet == CallbackManager::Error || callRet == CallbackManager::EntityNotFound)
                                    throw std::runtime_error("call to Save() failed");
                                std::string storage = this->_engine.GetInterpreter().GetSerializer().Serialize(entity->GetStorage(), true /* nilOnError */);

                                Tools::debug << ">> Save >> " << table << " >> Enabled Non-Positional Entity (id: " << it->first << ", pluginId: " << type.GetPluginId() << ", entityName: \"" << type.GetName() << "\", storage: " << storage.size() << " bytes)" << std::endl;
                                query->Bind(it->first).Bind(type.GetPluginId()).Bind(type.GetName()).Bind(0).Bind(storage).Bind(0).Bind(0).Bind(0).ExecuteNonSelect().Reset();
                            }
                            catch (std::exception& e)
                            {
                                Tools::log << "EntityManager::Save: Could not save non positional entity " << it->first << ": " << e.what() << std::endl;
                            }
                    }
            }
            // positional entities
            {
                auto it = this->_positionalEntities.begin();
                auto itEnd = this->_positionalEntities.end();
                for (; it != itEnd; ++it)
                    if (it->second)
                    {
                        PositionalEntity* entity = it->second;
                        auto const& pos = entity->GetPosition();
                        auto const& type = entity->GetType();
                        try
                        {
                            // XXX Save() database hook
                            // (peut delete l'entité, mais les iterateurs restent valident pour la map)
                            CallbackManager::Result callRet = this->CallEntityFunction(it->first, "Save", this->_engine.GetInterpreter().MakeNil(), this->_engine.GetInterpreter().MakeNil());
                            if (callRet == CallbackManager::Error || callRet == CallbackManager::EntityNotFound)
                                throw std::runtime_error("call to Save() failed");
                            std::string storage = this->_engine.GetInterpreter().GetSerializer().Serialize(entity->GetStorage(), true /* nilOnError */);

                            Tools::debug << ">> Save >> " << table << " >> Enabled Positional Entity (id: " << it->first << ", pluginId: " << type.GetPluginId() << ", entityName: \"" << type.GetName() << "\", storage: " << storage.size() << " bytes, x: " << pos.x << ", y: " << pos.y << ", z: " << pos.z << ")" << std::endl;
                            query->Bind(it->first).Bind(type.GetPluginId()).Bind(type.GetName()).Bind(0).Bind(storage).Bind(pos.x).Bind(pos.y).Bind(pos.z).ExecuteNonSelect().Reset();
                        }
                        catch (std::exception& e)
                        {
                            Tools::error << "EntityManager::Save: Could not save positional entity " << it->first << ": " << e.what() << std::endl;
                        }
                    }
            }
            // disabled entities
            {
                auto it = this->_disabledEntities.begin();
                auto itEnd = this->_disabledEntities.end();
                for (; it != itEnd; ++it)
                {
                    PositionalEntity* entity = it->second;
                    std::string storage = this->_engine.GetInterpreter().GetSerializer().Serialize(entity->GetStorage(), true /* nilOnError */);
                    auto const& pos = entity->GetPosition();
                    auto const& type = entity->GetType();
                    try
                    {
                        Tools::debug << ">> Save >> " << table << " >> Disabled Entity (id: " << it->first << ", pluginId: " << type.GetPluginId() << ", entityName: \"" << type.GetName() << "\", storage: " << storage.size() << " bytes, x: " << pos.x << ", y: " << pos.y << ", z: " << pos.z << ")" << std::endl;
                        query->Bind(it->first).Bind(type.GetPluginId()).Bind(type.GetName()).Bind(1).Bind(storage).Bind(pos.x).Bind(pos.y).Bind(pos.z).ExecuteNonSelect().Reset();
                    }
                    catch (std::exception& e)
                    {
                        Tools::error << "EntityManager::Save: Could not save disabled positional entity " << it->first << ": " << e.what() << std::endl;
                    }
                }
            }
        }

        // spawn events
        {
            std::string table = this->_engine.GetMap().GetName() + "_spawn_event";
            conn.CreateQuery("DELETE FROM " + table)->ExecuteNonSelect();
            auto query = conn.CreateQuery("INSERT INTO " + table + " (id, plugin_id, entity_name, arg, spawner_id, notification_callback_id, pos_x, pos_y, pos_z) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?);");
            unsigned int id = 0;
            auto it = this->_spawnEvents.begin();
            auto itEnd = this->_spawnEvents.end();
            for (; it != itEnd; ++it)
            {
                ++id; // juste pour conserver l'ordre au chargement
                auto e = *it;
                std::string arg = this->_engine.GetInterpreter().GetSerializer().Serialize(e->arg, true /* nilOnError */);
                auto const& pos = e->pos;
                try
                {
                    Tools::debug << ">> Save >> " << table << " >> Spawn Event (id: " << id << ", pluginId: " << e->pluginId << ", entityName: \"" << e->entityName << "\", spawnerId: " << e->spawnerId << ", notificationCallbackId: " << e->notificationCallbackId << ", x: " << pos.x << ", y: " << pos.y << ", z: " << pos.z << ")" << std::endl;
                    query->Bind(id).Bind(e->pluginId).Bind(e->entityName).Bind(arg).Bind(e->spawnerId).Bind(e->notificationCallbackId).Bind(pos.x).Bind(pos.y).Bind(pos.z).ExecuteNonSelect().Reset();
                }
                catch (std::exception& ex)
                {
                    Tools::error << "EntityManager::Save: Could not save spawn event for entity \"" << e->entityName << "\" (plugin " << e->pluginId << "): " << ex.what() << std::endl;
                }
            }
        }

        // kill events
        {
            std::string table = this->_engine.GetMap().GetName() + "_kill_event";
            conn.CreateQuery("DELETE FROM " + table)->ExecuteNonSelect();
            auto query = conn.CreateQuery("INSERT INTO " + table + " (id, target_id, arg, killer_id, notification_callback_id) VALUES (?, ?, ?, ?, ?);");
            unsigned int id = 0;
            auto it = this->_killEvents.begin();
            auto itEnd = this->_killEvents.end();
            for (; it != itEnd; ++it)
            {
                ++id; // juste pour conserver l'ordre au chargement
                auto e = *it;
                std::string arg = this->_engine.GetInterpreter().GetSerializer().Serialize(e->arg, true /* nilOnError */);
                try
                {
                    Tools::debug << ">> Save >> " << table << " >> Kill Event (id: " << id << ", targetId: " << e->targetId << ", arg: " << arg.size() << " bytes, killerId: " << e->killerId << ", notificationCallbackId: " << e->notificationCallbackId << ")" << std::endl;
                    query->Bind(id).Bind(e->targetId).Bind(arg).Bind(e->killerId).Bind(e->notificationCallbackId).ExecuteNonSelect().Reset();
                }
                catch (std::exception& ex)
                {
                    Tools::error << "EntityManager::Save: Could not save a kill event for entity " << e->targetId << ": " << ex.what() << std::endl;
                }
            }
        }
    }

    void EntityManager::Load(Tools::Database::IConnection& conn)
    {
        // entities
        {
            std::string table = this->_engine.GetMap().GetName() + "_entity";
            auto query = conn.CreateQuery("SELECT id, plugin_id, entity_name, disabled, storage, pos_x, pos_y, pos_z FROM " + table);
            while (auto row = query->Fetch())
            {
                Uint32 entityId = row->GetUint32(0);
                Uint32 pluginId = row->GetUint32(1);
                std::string entityName = row->GetString(2);
                bool disabled = row->GetUint32(3);
                Common::Position pos(row->GetDouble(5), row->GetDouble(6), row->GetDouble(7));
                try
                {
                    Tools::debug << "<< Load << " << table << " << " << (disabled ? "Disabled" : "Enabled") << " Entity (id: " << entityId << ", pluginId: " << pluginId << ", entityName: \"" << entityName << "\", storage: <lua>, x: " << pos.x << ", y: " << pos.y << ", z: " << pos.z << ")" << std::endl;
                    Entity* e = this->_CreateEntity(entityId, pluginId, entityName, this->IsEntityTypePositional(pluginId, entityName), pos);

                    // XXX Load() database hook
                    bool callLoad = !disabled;
                    e->SetStorage(this->_engine.GetInterpreter().GetSerializer().Deserialize(row->GetString(4)));
                    if (disabled)
                        try
                        {
                            this->DisableEntity(entityId, false /* callSave */); // fait comme si on désactivait l'entité, mais sans Save()
                        }
                        catch (std::exception&)
                        {
                            callLoad = true; // l'entité est crée mais n'a pas pu être désactivée : c'est donc une entité non positionnelle, on va appeler Load()
                                             // (ne devrait jamais arriver, seulement si un boloss change le flag 'disabled' dans la base)
                        }
                    if (callLoad)
                    {
                        CallbackManager::Result callRet = this->CallEntityFunction(entityId, "Load", this->_engine.GetInterpreter().MakeNil(), this->_engine.GetInterpreter().MakeNil());
                        if (callRet == CallbackManager::Error || callRet == CallbackManager::EntityNotFound)
                            throw std::runtime_error("call to Load() failed");
                    }
                }
                catch (std::exception& e)
                {
                    Tools::error << "EntityManager::Load: Could not load entity \"" << entityName << "\" (plugin " << pluginId << "): " << e.what() << std::endl;
                }
            }
        }

        // spawn events
        {
            std::string table = this->_engine.GetMap().GetName() + "_spawn_event";
            auto query = conn.CreateQuery("SELECT plugin_id, entity_name, arg, spawner_id, notification_callback_id, pos_x, pos_y, pos_z FROM " + table + " ORDER BY id;");
            while (auto row = query->Fetch())
            {
                Uint32 pluginId = row->GetUint32(0);
                std::string entityName = row->GetString(1);
                Uint32 spawnerId = row->GetUint32(3);
                Uint32 notificationCallbackId = row->GetUint32(4);
                Common::Position pos(row->GetDouble(5), row->GetDouble(6), row->GetDouble(7));
                try
                {
                    Tools::debug << "<< Load << " << table << " << Spawn Event (pluginId: " << pluginId << ", entityName: \"" << entityName << "\", spawnerId: " << spawnerId << ", notificationCallbackId: " << notificationCallbackId << ", x: " << pos.x << ", y: " << pos.y << ", z: " << pos.z << ")" << std::endl;
                    Tools::Lua::Ref arg = this->_engine.GetInterpreter().GetSerializer().Deserialize(row->GetString(2));
                    this->AddSpawnEvent(pluginId, entityName, arg, spawnerId, notificationCallbackId, pos);
                }
                catch (std::exception& e) // erreur de deserialization
                {
                    Tools::error << "EntityManager::Load: Could not load spawn event for entity \"" << entityName << "\" (plugin " << pluginId << "): " << e.what() << std::endl;
                }
            }
        }

        // kill events
        {
            std::string table = this->_engine.GetMap().GetName() + "_kill_event";
            auto query = conn.CreateQuery("SELECT target_id, arg, killer_id, notification_callback_id FROM " + table + " ORDER BY id;");
            while (auto row = query->Fetch())
            {
                Uint32 targetId = row->GetUint32(0);
                Uint32 killerId = row->GetUint32(2);
                Uint32 notificationCallbackId = row->GetUint32(3);
                try
                {
                    Tools::debug << "<< Load << " << table << " << Kill Event (targetId: " << targetId << ", arg: <lua>, killerId: " << killerId << ", notificationCallbackId: " << notificationCallbackId << ")" << std::endl;
                    Tools::Lua::Ref arg = this->_engine.GetInterpreter().GetSerializer().Deserialize(row->GetString(1));
                    this->AddKillEvent(targetId, arg, killerId, notificationCallbackId);
                }
                catch (std::exception& e) // erreur de deserialization
                {
                    Tools::error << "EntityManager::Load: Could not load kill event for entity " << targetId << ": " << e.what() << std::endl;
                }
            }
        }
    }

    void EntityManager::BootstrapPlugin(Uint32 pluginId, Tools::Database::IConnection& conn)
    {
        std::string table = this->_engine.GetMap().GetName() + "_initialized_plugin";
        auto query = conn.CreateQuery("SELECT 1 FROM " + table + " WHERE id = ?;");
        query->Bind(pluginId);
        if (query->Fetch())
        {
            Tools::debug << "<< Load << " << table << " << Initialized Plugin (pluginId: " << pluginId << ", no entity spawned)" << std::endl;
            return;
        }
        Tools::debug << "<< Load << " << table << " << Non-Initialized Plugin (pluginId: " << pluginId << ", spawning entity)" << std::endl;
        this->AddSpawnEvent(pluginId, "Init", this->_engine.GetInterpreter().MakeNil() /* arg */, 0 /* spawnerId */, 0 /* callbackId */);
    }

    Uint32 EntityManager::GetRunningPluginId() const
    {
        return this->_runningEntity ? this->_runningEntity->GetType().GetPluginId() : 0;
    }

    Entity const& EntityManager::GetEntity(Uint32 entityId) const throw(std::runtime_error)
    {
        auto it = this->_entities.find(entityId);
        if (it == this->_entities.end() || !it->second)
            throw std::runtime_error("EntityManager: Entity not found.");
        return *it->second;
    }

    void EntityManager::DisableEntity(Uint32 entityId, bool callSave /* = true */) throw(std::runtime_error)
    {
        // trouve l'entité (positonnelle)
        auto itPos = this->_positionalEntities.find(entityId);
        if (itPos == this->_positionalEntities.end())
            throw std::runtime_error("EntityManager: Could not disable entity: positional entity not found.");
        if (!itPos->second)
            throw std::runtime_error("EntityManager: Could not disable entity: positional entity already disabled.");

        // XXX Save() activation hook
        if (callSave)
        {
            CallbackManager::Result callRet = this->CallEntityFunction(entityId, "Save", this->_engine.GetInterpreter().MakeNil(), this->_engine.GetInterpreter().MakeNil());
            if (callRet == CallbackManager::Error || callRet == CallbackManager::EntityNotFound)
                throw std::runtime_error("EntityManager: Could not disable entity: call to Save() failed (entity deleted).");
        }

        // ajoute l'entité dans la map des entités désactivées
        assert(!this->_disabledEntities.count(entityId) && "une entité positionnelle était déjà dans la map des entités désactivées");
        this->_disabledEntities[entityId] = itPos->second;

        // met 0 dans la map des entités normales
        auto it = this->_entities.find(entityId);
        assert(it != this->_entities.end() && it->second && "une entité positionnelle n'était pas présente dans la map des entités normales au moment de sa désactivation");
        it->second = 0;

        // met 0 dans la map des entités positionnelles
        itPos->second = 0;
    }

    void EntityManager::EnableEntity(Uint32 entityId) throw(std::runtime_error)
    {
        // trouve l'entité désactivée
        auto itDisabled = this->_disabledEntities.find(entityId);
        if (itDisabled == this->_disabledEntities.end())
            throw std::runtime_error("EntityManager: Could not enable entity: positional entity not found.");

        // restauration dans la map des entités normales
        auto it = this->_entities.find(entityId);
        assert(it != this->_entities.end() && !it->second && "pas de pointeur nul dans la map des entités normales au moment de la réactivation d'une entité positionnelle");
        it->second = itDisabled->second;

        // restauration dans la map des entités positionnelles
        auto itPos = this->_positionalEntities.find(entityId);
        assert(itPos != this->_positionalEntities.end() && !itPos->second && "pas de pointeur nul dans la map des entités normales au moment de la réactivation d'une entité positionnelle");
        itPos->second = itDisabled->second;

        // enlève l'entité de la map des entités désactivées
        this->_disabledEntities.erase(itDisabled);

        // XXX Load() deactivation hook
        CallbackManager::Result callRet = this->CallEntityFunction(entityId, "Load", this->_engine.GetInterpreter().MakeNil(), this->_engine.GetInterpreter().MakeNil());
        if (callRet == CallbackManager::Error || callRet == CallbackManager::EntityNotFound)
            throw std::runtime_error("EntityManager: Could not enable entity: call to Load() failed (entity deleted).");
    }

    bool EntityManager::IsEntityTypePositional(Uint32 pluginId, std::string const& entityName) const
    {
        auto itPlugin = this->_entityTypes.find(pluginId);
        if (itPlugin != this->_entityTypes.end())
        {
            auto itType = itPlugin->second.find(entityName);
            if (itType != itPlugin->second.end())
                return itType->second->IsPositional();
        }
        return false;
    }

    Entity* EntityManager::_CreateEntity(Uint32 entityId, Uint32 pluginId, std::string entityName, bool positional /* = false */, Common::Position const& pos /* = Common::Position() */) throw(std::runtime_error)
    {
        // check d'id
        if (this->_entities.count(entityId) || this->_positionalEntities.count(entityId))
            throw std::runtime_error("an entity with this id already exists");

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

        // allocation
        Entity* entity;
        if (itType->second->IsPositional())
        {
            PositionalEntity* positionalEntity = new PositionalEntity(this->_engine, entityId, itType->second, pos);
            assert(!this->_positionalEntities.count(entityId) && "impossible de créer une nouvelle entité car l'id est déjà utilisé dans la map des entités positionnelles");
            this->_positionalEntities[entityId] = positionalEntity;
            entity = positionalEntity;
        }
        else
            entity = new Entity(this->_engine, entityId, itType->second);
        assert(!this->_entities.count(entityId) && "impossible de créer une nouvelle entité car l'id est déjà utilisé dans la map des entités normales");
        this->_entities[entityId] = entity;

        Tools::debug << "EntityManager::_CreateEntity: New entity \"" << entityName << "\" (plugin " << pluginId << ") created (" << entityId << ").\n";
        return entity;
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
                throw std::runtime_error("Server.Entity.Register[Positional]: Invalid entity name \"" + entityName + "\"");
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
        bool first = true;
        auto it = this->_entities.begin();
        auto itEnd = this->_entities.end();
        for (; it != itEnd; ++it)
            if (it->second)
            {
                if (!first)
                    json += ",\n";
                first = false;
                std::string storage;
                if (it->second->GetSelf().IsTable())
                    try
                    {
                        storage = this->_engine.GetInterpreter().GetSerializer().Serialize(it->second->GetSelf()["storage"]);
                    }
                    catch (std::exception& e)
                    {
                        // normalement on utilise nilOnError pour le storage, mais ici on debug donc on affiche plus de trucs
                        storage = "Serialization error: " + std::string(e.what());
                    }
                json +=
                    "\t{\n"
                    "\t\t\"id\": " + Tools::ToString(it->first) + ",\n" +
                    "\t\t\"type\": \"" + it->second->GetType().GetName() + "\",\n" +
                    "\t\t\"plugin\": \"" + this->_engine.GetWorld().GetPluginManager().GetPluginIdentifier(it->second->GetType().GetPluginId()) + "\",\n" +
                    "\t\t\"positional\": " + (it->second->GetType().IsPositional() ? "true" : "false") + ",\n" +
                    "\t\t\"storage\": \"" + Rcon::ToJsonStr(storage) + "\"\n" +
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
