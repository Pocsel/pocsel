#include "server/precompiled.hpp"

#include "server/Server.hpp"
#include "server/Settings.hpp"
#include "server/game/engine/EntityManager.hpp"
#include "server/game/engine/Engine.hpp"
#include "server/game/engine/Entity.hpp"
#include "server/game/engine/PositionalEntity.hpp"
#include "server/game/engine/DoodadManager.hpp"
#include "server/game/engine/EntityType.hpp"
#include "server/game/engine/PhysicsManager.hpp"
#include "server/game/Game.hpp"
#include "server/game/World.hpp"
#include "server/game/PluginManager.hpp"
#include "tools/lua/Interpreter.hpp"
#include "tools/lua/MetaTable.hpp"
#include "tools/database/sqlite/Connection.hpp"
#include "server/game/map/Map.hpp"
#include "common/FieldUtils.hpp"
#include "server/rcon/ToJsonStr.hpp"
#include "server/rcon/EntityManager.hpp"

namespace Server { namespace Game { namespace Engine {

    EntityManager::EntityManager(Engine& engine) :
        _engine(engine),
        _nextEntityId(1) // la première entité sera la 1, 0 est la valeur spéciale "pas d'entité"
    {
        Tools::debug << "EntityManager::EntityManager()\n";
        auto& i = this->_engine.GetInterpreter();

        this->_weakEntityRefManager = new Tools::Lua::WeakResourceRefManager<WeakEntityRef, EntityManager>(
                i, /* interpreter */
                *this /* resource manager */,
                WeakEntityRef() /* invalid resource */,
                this->_engine.GetWorld().GetGame().GetServer().GetSettings().debug /* use fake references */);

        auto namespaceTable = i.Globals()["Server"].Set("Entity", i.MakeTable());

        namespaceTable.Set("GetEntityById", i.MakeFunction(std::bind(&EntityManager::_ApiGetEntityById, this, std::placeholders::_1)));
        namespaceTable.Set("Spawn", i.MakeFunction(std::bind(&EntityManager::_ApiSpawn, this, std::placeholders::_1)));
        namespaceTable.Set("SetPos", i.MakeFunction(std::bind(&EntityManager::_ApiSetPos, this, std::placeholders::_1)));
        namespaceTable.Set("GetPos", i.MakeFunction(std::bind(&EntityManager::_ApiGetPos, this, std::placeholders::_1)));
        namespaceTable.Set("Save", i.MakeFunction(std::bind(&EntityManager::_ApiSave, this, std::placeholders::_1)));
        namespaceTable.Set("Load", i.MakeFunction(std::bind(&EntityManager::_ApiLoad, this, std::placeholders::_1)));
        namespaceTable.Set("Kill", i.MakeFunction(std::bind(&EntityManager::_ApiKill, this, std::placeholders::_1)));
        namespaceTable.Set("Register", i.MakeFunction(std::bind(&EntityManager::_ApiRegister, this, std::placeholders::_1)));
        namespaceTable.Set("GetWeakPointer", i.MakeFunction(std::bind(&EntityManager::_ApiGetWeakPointer, this, std::placeholders::_1)));

        // positionnal
        namespaceTable.Set("RegisterPositional", i.MakeFunction(std::bind(&EntityManager::_ApiRegisterPositional, this, std::placeholders::_1)));
        namespaceTable.Set("SetPos", i.MakeFunction(std::bind(&EntityManager::_ApiSetPos, this, std::placeholders::_1)));
        namespaceTable.Set("GetPos", i.MakeFunction(std::bind(&EntityManager::_ApiGetPos, this, std::placeholders::_1)));
        namespaceTable.Set("SetSpeed", i.MakeFunction(std::bind(&EntityManager::_ApiSetSpeed, this, std::placeholders::_1)));
        namespaceTable.Set("GetSpeed", i.MakeFunction(std::bind(&EntityManager::_ApiGetSpeed, this, std::placeholders::_1)));
        namespaceTable.Set("SetVel", i.MakeFunction(std::bind(&EntityManager::_ApiSetSpeed, this, std::placeholders::_1)));
        namespaceTable.Set("GetVel", i.MakeFunction(std::bind(&EntityManager::_ApiGetSpeed, this, std::placeholders::_1)));
        namespaceTable.Set("SetAccel", i.MakeFunction(std::bind(&EntityManager::_ApiSetAccel, this, std::placeholders::_1)));
        namespaceTable.Set("GetAccel", i.MakeFunction(std::bind(&EntityManager::_ApiGetAccel, this, std::placeholders::_1)));
    }

    EntityManager::~EntityManager()
    {
        Tools::debug << "EntityManager::~EntityManager()\n";
        // entities
        auto itEntity = this->_entities.begin();
        auto itEntityEnd = this->_entities.end();
        for (; itEntity != itEntityEnd; ++itEntity)
            Tools::Delete(itEntity->second);
        // disabled entities
        auto itDisabledEntity = this->_disabledEntities.begin();
        auto itDisabledEntityEnd = this->_disabledEntities.end();
        for (; itDisabledEntity != itDisabledEntityEnd; ++itDisabledEntity)
            Tools::Delete(itDisabledEntity->second);
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
        // resource manager
        Tools::Delete(this->_weakEntityRefManager);
    }

    Tools::Lua::Ref EntityManager::WeakEntityRef::GetReference(EntityManager const& entityManager) const
    {
        return entityManager.GetEntity(this->entityId).GetSelf();
    }

    std::string EntityManager::WeakEntityRef::Serialize(EntityManager const& entityManager) const
    {
        return "return Server.Entity.GetWeakPointer(" + Tools::ToString(this->entityId) + ")";
    }

    bool EntityManager::WeakEntityRef::operator <(WeakEntityRef const& rhs) const
    {
        if (this->disabled && !rhs.disabled)
            return true;
        else if (!this->disabled && rhs.disabled)
            return false;
        else
            return this->entityId < rhs.entityId;
    }

    CallbackManager::Result EntityManager::CallEntityFunction(Uint32 entityId, std::string const& function, Tools::Lua::Ref const& arg, Tools::Lua::Ref const& bonusArg, Tools::Lua::Ref* ret /* = 0 */)
    {
        auto it = this->_entities.find(entityId);
        if (it == this->_entities.end() || !it->second)
        {
            Tools::error << "EntityManager::CallEntityFunction: Call to \"" << function << "\" for entity " << entityId << " failed: entity not found.\n";
            return CallbackManager::EntityNotFound;
        }
        try
        {
            auto prototype = it->second->GetType().GetPrototype();
            if (!prototype.IsTable())
                throw std::runtime_error("prototype is not a table"); // le moddeur a fait de la merde avec son type...
            auto f = it->second->GetType().GetPrototype()[function];
            if (f.IsFunction())
            {
                if (bonusArg.Exists())
                    ret ? *ret = f(it->second->GetSelf(), arg, bonusArg) : f(it->second->GetSelf(), arg, bonusArg);
                else
                    ret ? *ret = f(it->second->GetSelf(), arg) : f(it->second->GetSelf(), arg);
            }
            else
            {
                Tools::error << "EntityManager::CallEntityFunction: Call to \"" << function << "\" for entity " << entityId << " (\"" << it->second->GetType().GetName() << "\") failed: function not found (type: " << f.GetTypeName() << ").\n";
                return CallbackManager::FunctionNotFound;
            }
        }
        catch (std::exception& e)
        {
            Tools::error << "EntityManager::CallEntityFunction: Fatal (entity deleted): Call to \"" << function << "\" for entity " << entityId << " (\"" << it->second->GetType().GetName() << "\") failed: " << e.what() << std::endl;
            this->_DeleteEntity(it->first, it->second);
            return CallbackManager::Error;
        }
        //Tools::debug << "EntityManager::CallEntityFunction: Function \"" << function << "\" called for entity " << entityId << " (\"" << it->second->GetType().GetName() << "\").\n";
        return CallbackManager::Ok;
    }

    void EntityManager::AddSpawnEvent(Uint32 pluginId, std::string const& entityName, Tools::Lua::Ref const& arg, Uint32 notificationCallbackId, bool hasPosition /* = false */, Common::Position const& pos /* = Common::Position() */)
    {
        this->_spawnEvents.push_back(new SpawnEvent(pluginId, entityName, arg, notificationCallbackId, hasPosition, pos));
    }

    void EntityManager::AddKillEvent(Uint32 entityId, Tools::Lua::Ref const& arg, Uint32 notificationCallbackId)
    {
        this->_killEvents.push_back(new KillEvent(entityId, arg, notificationCallbackId));
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
                        || this->_entities.count(this->_nextEntityId))
                    ++this->_nextEntityId;
                Uint32 newId = this->_nextEntityId++;

                // XXX Spawn() hook
                Tools::Lua::Ref ret(this->_engine.GetInterpreter().GetState());
                this->_CreateEntity(newId, e->pluginId, e->entityName, e->hasPosition, e->pos);
                if (this->CallEntityFunction(newId, "Spawn", e->arg, this->_engine.GetInterpreter().MakeNil() /* bonus */, &ret) == CallbackManager::Ok)
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
            auto it = this->_entities.find(e->entityId);
            auto resultTable = this->_engine.GetInterpreter().MakeTable();
            resultTable.Set("entityId", this->_engine.GetInterpreter().MakeNumber(e->entityId));
            if (it != this->_entities.end() && it->second)
            {
                Tools::Lua::Ref ret(this->_engine.GetInterpreter().GetState());

                // XXX Die() hook
                if (this->CallEntityFunction(e->entityId, "Die", e->arg, this->_engine.GetInterpreter().MakeNil() /* bonus */, &ret) == CallbackManager::Ok)
                    resultTable.Set("ret", this->_engine.GetInterpreter().GetSerializer().MakeSerializableCopy(ret, true));
                this->_DeleteEntity(it->first, it->second);
                resultTable.Set("success", this->_engine.GetInterpreter().MakeBoolean(true)); // meme si le call a fail, l'entité est tuée quand même alors on retourne true
                notifications.push(std::make_pair(e->notificationCallbackId, resultTable));
            }
            else
            {
                Tools::error << "EntityManager::DispatchKillEvents: Cannot kill entity " << e->entityId << ": entity not found.\n";
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
                auto itTmp = this->_entities.begin();
                auto itEnd = this->_entities.end();
                while (itTmp != itEnd)
                {
                    auto it = itTmp++;
                    Entity* entity = it->second;
                    if (entity)
                    {
                        Uint32 id = it->first;
                        auto const& type = entity->GetType();
                        if (!type.IsPositional()) // saute les entités positionnelles, elles sont gérées differement après
                            try
                            {
                                Tools::Lua::Ref ret(this->_engine.GetInterpreter().GetState());

                                // XXX Save() database hook
                                CallbackManager::Result callRet = this->CallEntityFunction(id, "Save", this->_engine.GetInterpreter().MakeBoolean(false) /* chunkUnloaded */, this->_engine.GetInterpreter().MakeNil(), &ret);
                                if (callRet == CallbackManager::Error || callRet == CallbackManager::EntityNotFound)
                                    throw std::runtime_error("call to Save() failed");
                                std::string storage = this->_engine.GetInterpreter().GetSerializer().Serialize(entity->GetStorage(), true /* nilOnError */);

                                if (ret.ToBoolean())
                                {
                                    Tools::debug << ">> Save >> " << table << " >> Enabled Non-Positional Entity " << id << " not saved." << std::endl;
                                    this->_DeleteEntity(id, entity);
                                }
                                else
                                {
                                    Tools::debug << ">> Save >> " << table << " >> Enabled Non-Positional Entity (id: " << id << ", pluginId: " << type.GetPluginId() << ", entityName: \"" << type.GetName() << "\", storage: " << storage.size() << " bytes)" << std::endl;
                                    query->Bind(id).Bind(type.GetPluginId()).Bind(type.GetName()).Bind(0).Bind(storage).Bind(0).Bind(0).Bind(0).ExecuteNonSelect().Reset();
                                }
                            }
                            catch (std::exception& e)
                            {
                                Tools::log << "EntityManager::Save: Could not save non positional entity " << id << ": " << e.what() << std::endl;
                            }
                    }
                }
            }
            // positional entities
            {
                auto itTmp = this->_positionalEntities.begin();
                auto itEnd = this->_positionalEntities.end();
                while (itTmp != itEnd)
                {
                    auto it = itTmp++;
                    PositionalEntity* entity = it->second;
                    if (entity)
                    {
                        Uint32 id = it->first;
                        auto const& pos = entity->GetPosition();
                        auto const& type = entity->GetType();
                        try
                        {
                            Tools::Lua::Ref ret(this->_engine.GetInterpreter().GetState());

                            // XXX Save() database hook
                            CallbackManager::Result callRet = this->CallEntityFunction(id, "Save", this->_engine.GetInterpreter().MakeBoolean(false) /* chunkUnloaded */, this->_engine.GetInterpreter().MakeNil(), &ret);
                            if (callRet == CallbackManager::Error || callRet == CallbackManager::EntityNotFound)
                                throw std::runtime_error("call to Save() failed");
                            std::string storage = this->_engine.GetInterpreter().GetSerializer().Serialize(entity->GetStorage(), true /* nilOnError */);

                            if (ret.ToBoolean())
                            {
                                Tools::debug << ">> Save >> " << table << " >> Enabled Positional Entity " << id << " not saved." << std::endl;
                                this->_DeleteEntity(id, it->second);
                            }
                            else
                            {
                                Tools::debug << ">> Save >> " << table << " >> Enabled Positional Entity (id: " << id << ", pluginId: " << type.GetPluginId() << ", entityName: \"" << type.GetName() << "\", storage: " << storage.size() << " bytes, x: " << pos.x << ", y: " << pos.y << ", z: " << pos.z << ")" << std::endl;
                                query->Bind(id).Bind(type.GetPluginId()).Bind(type.GetName()).Bind(0).Bind(storage).Bind(pos.x).Bind(pos.y).Bind(pos.z).ExecuteNonSelect().Reset();
                            }
                        }
                        catch (std::exception& e)
                        {
                            Tools::error << "EntityManager::Save: Could not save positional entity " << id << ": " << e.what() << std::endl;
                        }
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
            auto query = conn.CreateQuery("INSERT INTO " + table + " (id, plugin_id, entity_name, arg, notification_callback_id, has_position, pos_x, pos_y, pos_z) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?);");
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
                    Tools::debug << ">> Save >> " << table << " >> Spawn Event (id: " << id << ", pluginId: " << e->pluginId << ", entityName: \"" << e->entityName << "\", notificationCallbackId: " << e->notificationCallbackId << ", hasPosition: " << (e->hasPosition ? "yes" : "no") << ", x: " << pos.x << ", y: " << pos.y << ", z: " << pos.z << ")" << std::endl;
                    query->Bind(id).Bind(e->pluginId).Bind(e->entityName).Bind(arg).Bind(e->notificationCallbackId).Bind(e->hasPosition ? 1 : 0).Bind(pos.x).Bind(pos.y).Bind(pos.z).ExecuteNonSelect().Reset();
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
            auto query = conn.CreateQuery("INSERT INTO " + table + " (id, entity_id, arg, notification_callback_id) VALUES (?, ?, ?, ?);");
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
                    Tools::debug << ">> Save >> " << table << " >> Kill Event (id: " << id << ", entityId: " << e->entityId << ", arg: " << arg.size() << " bytes, notificationCallbackId: " << e->notificationCallbackId << ")" << std::endl;
                    query->Bind(id).Bind(e->entityId).Bind(arg).Bind(e->notificationCallbackId).ExecuteNonSelect().Reset();
                }
                catch (std::exception& ex)
                {
                    Tools::error << "EntityManager::Save: Could not save a kill event for entity " << e->entityId << ": " << ex.what() << std::endl;
                }
            }
        }
    }

    void EntityManager::Load(Tools::Database::IConnection& conn)
    {
        // entities
        {
            Uint32 maxId = 0;
            std::string table = this->_engine.GetMap().GetName() + "_entity";
            auto query = conn.CreateQuery("SELECT id, plugin_id, entity_name, disabled, storage, pos_x, pos_y, pos_z FROM " + table);
            while (auto row = query->Fetch())
            {
                Uint32 entityId = row->GetUint32(0);
                if (entityId > maxId)
                    maxId = entityId;
                Uint32 pluginId = row->GetUint32(1);
                std::string entityName = row->GetString(2);
                bool disabled = row->GetUint32(3) != 0;
                Common::Position pos(row->GetDouble(5), row->GetDouble(6), row->GetDouble(7));
                try
                {
                    Tools::debug << "<< Load << " << table << " << " << (disabled ? "Disabled" : "Enabled") << " Entity (id: " << entityId << ", pluginId: " << pluginId << ", entityName: \"" << entityName << "\", storage: <lua>, x: " << pos.x << ", y: " << pos.y << ", z: " << pos.z << ")" << std::endl;
                    Tools::Lua::Ref storage = this->_engine.GetInterpreter().GetSerializer().Deserialize(row->GetString(4));
                    Entity* e = this->_CreateEntity(entityId, pluginId, entityName, true, pos);
                    e->SetStorage(storage);

                    // XXX Load() database hook
                    bool callLoad = !disabled;
                    if (disabled)
                        try
                        {
                            // fait comme si on désactivait l'entité, mais sans Save(), et sans désactivation de ses doodads
                            this->DisableEntity(entityId, false /* chunkUnloaded */);
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
            this->_nextEntityId = maxId + 1; // utile si jamais un script Lua fait des if sur des id d'entités
        }

        // spawn events
        {
            std::string table = this->_engine.GetMap().GetName() + "_spawn_event";
            auto query = conn.CreateQuery("SELECT plugin_id, entity_name, arg, notification_callback_id, has_position, pos_x, pos_y, pos_z FROM " + table + " ORDER BY id;");
            while (auto row = query->Fetch())
            {
                Uint32 pluginId = row->GetUint32(0);
                std::string entityName = row->GetString(1);
                Uint32 notificationCallbackId = row->GetUint32(3);
                bool hasPosition = row->GetUint32(4) == 1;
                Common::Position pos(row->GetDouble(4), row->GetDouble(5), row->GetDouble(7));
                try
                {
                    Tools::debug << "<< Load << " << table << " << Spawn Event (pluginId: " << pluginId << ", entityName: \"" << entityName << "\", notificationCallbackId: " << notificationCallbackId << ", hasPosition: " << (hasPosition ? "yes" : "no") << ", x: " << pos.x << ", y: " << pos.y << ", z: " << pos.z << ")" << std::endl;
                    Tools::Lua::Ref arg = this->_engine.GetInterpreter().GetSerializer().Deserialize(row->GetString(2));
                    this->AddSpawnEvent(pluginId, entityName, arg, notificationCallbackId, hasPosition, pos);
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
            auto query = conn.CreateQuery("SELECT entity_id, arg, notification_callback_id FROM " + table + " ORDER BY id;");
            while (auto row = query->Fetch())
            {
                Uint32 entityId = row->GetUint32(0);
                Uint32 notificationCallbackId = row->GetUint32(2);
                try
                {
                    Tools::debug << "<< Load << " << table << " << Kill Event (entityId: " << entityId << ", arg: <lua>, notificationCallbackId: " << notificationCallbackId << ")" << std::endl;
                    Tools::Lua::Ref arg = this->_engine.GetInterpreter().GetSerializer().Deserialize(row->GetString(1));
                    this->AddKillEvent(entityId, arg, notificationCallbackId);
                }
                catch (std::exception& e) // erreur de deserialization
                {
                    Tools::error << "EntityManager::Load: Could not load kill event for entity " << entityId << ": " << e.what() << std::endl;
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
        this->AddSpawnEvent(pluginId, "Init", this->_engine.GetInterpreter().MakeNil() /* arg */, 0 /* callbackId */);
    }

    Entity const& EntityManager::GetEntity(Uint32 entityId) const throw(std::runtime_error)
    {
        auto it = this->_entities.find(entityId);
        if (it == this->_entities.end() || !it->second)
            throw std::runtime_error("EntityManager::GetEntity: Entity " + Tools::ToString(entityId) + " not found.");
        return *it->second;
    }

    Entity const& EntityManager::GetEntity(Tools::Lua::Ref const& ref) const throw(std::runtime_error)
    {
        if (ref.IsTable())
            return this->GetEntity(ref["id"].Check<Uint32>("EntityManager::GetEntity: Table argument has no id number field"));
        else if (ref.IsNumber())
            return this->GetEntity(ref.To<Uint32>());
        else if (ref.IsUserData())
        {
            WeakEntityRef* e = ref.Check<WeakEntityRef*>("EntityManager::GetEntity: Userdata argument is not of WeakEntityRef type");
            if (e->IsValid(*this))
                return this->GetEntity(e->entityId);
            else
                throw std::runtime_error("EntityManager::GetEntity: This reference was invalidated - you must not keep true references to entities, only weak references");
        }
        else
            throw std::runtime_error("EntityManager::GetEntity: Invalid argument type " + ref.GetTypeName() + " given");
    }

    PositionalEntity& EntityManager::GetPositionalEntity(Uint32 entityId) throw(std::runtime_error)
    {
        auto it = this->_positionalEntities.find(entityId);
        if (it == this->_positionalEntities.end() || !it->second)
            throw std::runtime_error("EntityManager::GetPositionalEntity: Positional " + Tools::ToString(entityId) + " entity not found.");
        return *it->second;
    }

    PositionalEntity const& EntityManager::GetPositionalEntity(Uint32 entityId) const throw(std::runtime_error)
    {
        auto it = this->_positionalEntities.find(entityId);
        if (it == this->_positionalEntities.end() || !it->second)
            throw std::runtime_error("EntityManager::GetPositionalEntity: Positional " + Tools::ToString(entityId) + " entity not found.");
        return *it->second;
    }

    PositionalEntity const& EntityManager::GetPositionalEntity(Tools::Lua::Ref const& ref) const throw(std::runtime_error)
    {
        if (ref.IsTable())
            return this->GetPositionalEntity(ref["id"].Check<Uint32>("EntityManager::GetPositionalEntity: Table argument has no id number field"));
        else if (ref.IsNumber())
            return this->GetPositionalEntity(ref.To<Uint32>());
        else if (ref.IsUserData())
        {
            WeakEntityRef* e = ref.Check<WeakEntityRef*>("EntityManager::GetPositionalEntity: Userdata argument is not of WeakEntityRef type");
            if (e->IsValid(*this))
                return this->GetPositionalEntity(e->entityId);
            else
                throw std::runtime_error("EntityManager::GetPositionalEntity: This reference was invalidated - you must not keep true references to entities, only weak references");
        }
        else
            throw std::runtime_error("EntityManager::GetPositionalEntity: Argument of type " + ref.GetTypeName() + " given");
    }

    PositionalEntity& EntityManager::GetDisabledEntity(Uint32 entityId) throw(std::runtime_error)
    {
        auto it = this->_disabledEntities.find(entityId);
        if (it == this->_disabledEntities.end())
            throw std::runtime_error("EntityManager::GetDisabledEntity: Disabled entity not found.");
        return *it->second;
    }

    PositionalEntity const& EntityManager::GetDisabledEntity(Uint32 entityId) const throw(std::runtime_error)
    {
        auto it = this->_disabledEntities.find(entityId);
        if (it == this->_disabledEntities.end())
            throw std::runtime_error("EntityManager::GetDisabledEntity: Disabled entity not found.");
        return *it->second;
    }

    bool EntityManager::IsEntityPositional(Uint32 entityId) const
    {
        return this->_positionalEntities.count(entityId) > 0;
    }

    void EntityManager::DisableEntity(Uint32 entityId, bool chunkUnloaded /* = true */) throw(std::runtime_error)
    {
        // trouve l'entité (positonnelle)
        auto itPos = this->_positionalEntities.find(entityId);
        if (itPos == this->_positionalEntities.end())
            throw std::runtime_error("EntityManager::DisableEntity: Could not disable entity: positional entity not found.");
        if (!itPos->second)
            throw std::runtime_error("EntityManager::DisableEntity: Could not disable entity: positional entity already disabled.");

        if (chunkUnloaded)
        {
            Tools::Lua::Ref ret(this->_engine.GetInterpreter().GetState());

            // XXX Save() deactivation hook
            CallbackManager::Result callRet = this->CallEntityFunction(entityId, "Save", this->_engine.GetInterpreter().MakeBoolean(true) /* chunkUnloaded */, this->_engine.GetInterpreter().MakeNil(), &ret);
            if (callRet == CallbackManager::Error || callRet == CallbackManager::EntityNotFound)
                throw std::runtime_error("EntityManager::DisableEntity: Could not disable entity: call to Save() failed (entity deleted).");

            if (ret.ToBoolean())
            {
                this->_DeleteEntity(entityId, itPos->second);
                throw std::runtime_error("EntityManager::DisableEntity: Entity " + Tools::ToString(entityId) + " does not want to be saved.");
            }

            // désactive les doodads gérés par cette entité
            this->_engine.GetDoodadManager().DisableDoodadsOfEntity(entityId);
        }

        // donne au garbage collector les variables de cette entité
        itPos->second->Disable();

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

        // recrée la structure de base de l'entité
        itPos->second->Enable();

        // active les doodads gérés par cette entité
        this->_engine.GetDoodadManager().EnableDoodadsOfEntity(entityId);

        // XXX Load() activation hook
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

    void EntityManager::UpdatePositionalEntitiesPlayers()
    {
        for (auto it = this->_positionalEntities.begin(), ite = this->_positionalEntities.end(); it != ite; ++it)
        {
            PositionalEntity* entity = it->second;

            entity->UpdatePlayers();
        }
    }

    void EntityManager::EntityRemovedForPlayer(Uint32 playerId, Uint32 entityId)
    {
        auto it = this->_positionalEntities.find(entityId);
        if (it != this->_positionalEntities.end() && it->second)
            it->second->RemovePlayer(playerId);
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

    Entity* EntityManager::_CreateEntity(Uint32 entityId, Uint32 pluginId, std::string entityName, bool hasPosition /* = false */, Common::Position const& pos /* = Common::Position() */) throw(std::runtime_error)
    {
        // check d'id
        if (this->_entities.count(entityId) || this->_positionalEntities.count(entityId))
            throw std::runtime_error("an entity with id " + Tools::ToString(entityId) + " already exists");

        // trouve le plugin
        auto itPlugin = this->_entityTypes.find(pluginId);
        if (itPlugin == this->_entityTypes.end())
            throw std::runtime_error("unknown plugin " + Tools::ToString(pluginId));

        // trouve le prototype
        auto itType = itPlugin->second.find(entityName);
        if (itType == itPlugin->second.end())
            throw std::runtime_error("unknown entity \"" + entityName + "\" in plugin " + Tools::ToString(pluginId));

        // une position est necessaire si c'est une entité positionnelle
        if (itType->second->IsPositional() && !hasPosition)
            throw std::runtime_error("no position given for creating a positional entity \"" + entityName + "\" in plugin " + Tools::ToString(pluginId));

        // allocation
        Entity* entity;
        if (itType->second->IsPositional())
        {
            Common::Physics::Node position;
            position.position = pos;
            PositionalEntity* positionalEntity = new PositionalEntity(this->_engine.GetPhysicsManager().GetWorld(), this->_engine, entityId, *itType->second, position);
            assert(!this->_positionalEntities.count(entityId) && "impossible de créer une nouvelle entité car l'id est déjà utilisé dans la map des entités positionnelles");
            this->_positionalEntities[entityId] = positionalEntity;
            entity = positionalEntity;

            // XXX test
            auto const& players = this->_engine.GetMap().GetPlayers();
            auto it = players.begin();
            auto itEnd = players.end();
            for (; it != itEnd; ++it)
                positionalEntity->AddPlayer(it->first);
        }
        else
            entity = new Entity(this->_engine, entityId, *itType->second);
        assert(!this->_entities.count(entityId) && "impossible de créer une nouvelle entité car l'id est déjà utilisé dans la map des entités normales");
        this->_entities[entityId] = entity;

        Tools::debug << "EntityManager::_CreateEntity: New " << (itType->second->IsPositional() ? "positional " : "") << "entity \"" << entityName << "\" (plugin " << pluginId << ") created (id: " << entityId << (hasPosition ? ", x: " + Tools::ToString(pos.x) + ", y: " + Tools::ToString(pos.y) + ", z: " + Tools::ToString(pos.z) : "") << ").\n";
        return entity;
    }

    void EntityManager::_DeleteEntity(Uint32 id, Entity* entity)
    {
        if (entity->GetType().IsPositional())
        {
            size_t nbErased = this->_positionalEntities.erase(id);
            assert(nbErased == 1 && "j'ai voulu supprimer une entité positionnelle mais elle était pas dans sa map");
            (void)nbErased; // pour le warning unused
            this->_engine.GetDoodadManager().DeleteDoodadsOfEntity(id);
        }
        this->_entities.erase(id);
        Tools::Delete(entity);
    }

    void EntityManager::_ApiGetEntityById(Tools::Lua::CallHelper& helper)
    {
        Uint32 entityId = helper.PopArg("Server.Entity.GetEntityById: Missing argument \"entityId\"").Check<Uint32>("Server.Entity.GetEntityById: Argument \"entityId\" must be a number");
        auto it = this->_entities.find(entityId);
        if (it == this->_entities.end() || !it->second)
        {
            Tools::error << "EntityManager::_ApiGetEntityById: Entity " << entityId << " not found, invalid resource returned." << std::endl;
            helper.PushRet(this->_engine.GetInterpreter().MakeNil());
            return;
        }
        helper.PushRet(this->_weakEntityRefManager->GetWeakReference(it->second->GetWeakReferenceId()));
    }

    void EntityManager::_ApiSpawn(Tools::Lua::CallHelper& helper)
    {
        bool hasPosition = false;
        Common::Position pos;
        if (helper.GetNbArgs() && helper.GetArgList().front().IsTable())
        {
            hasPosition = true;
            pos = helper.PopArg().Check<glm::dvec3>();
        }
        std::string resourceName = helper.PopArg("Server.Entity.Spawn: Missing argument \"entityName\"").CheckString("Server.Entity.Spawn: Argument \"entityName\" must be a string");
        std::string pluginName = Common::FieldUtils::GetPluginNameFromResource(resourceName);
        std::string entityName = Common::FieldUtils::GetResourceNameFromResource(resourceName);
        Uint32 pluginId = this->_engine.GetWorld().GetPluginManager().GetPluginId(pluginName);
        Tools::Lua::Ref arg(this->_engine.GetInterpreter().GetState());
        Uint32 cbTargetId = 0;
        std::string cbFunction;
        Tools::Lua::Ref cbArg(this->_engine.GetInterpreter().GetState());
        if (helper.GetNbArgs())
        {
            arg = helper.PopArg();
            if (helper.GetNbArgs())
            {
                cbTargetId = helper.PopArg().Check<Uint32>("Server.Entity.Spawn: Argument \"cbTarget\" must be a number");
                cbFunction = helper.PopArg("Server.Entity.Spawn: Missing argument \"cbFunction\"").CheckString("Server.Entity.Spawn: Argument \"cbFunction\" must be a string");
                if (helper.GetNbArgs())
                    cbArg = helper.PopArg();
            }
        }
        Uint32 callbackId = 0;
        if (cbTargetId)
            callbackId = this->_engine.GetCallbackManager().MakeCallback(cbTargetId, cbFunction, cbArg);
        this->AddSpawnEvent(pluginId, entityName, arg, callbackId, hasPosition, pos);
    }

    void EntityManager::_ApiSave(Tools::Lua::CallHelper& helper)
    {
        Uint32 entityId = helper.PopArg("Server.Entity.Save: Missing argument \"target\"").To<Uint32>();
        auto it = this->_entities.find(entityId);
        if (it == this->_entities.end() || !it->second)
        {
            Tools::error << "EntityManager::_ApiSave: Entity " << entityId << " not found, cannot save." << std::endl;
            return;
        }
        it->second->SaveToStorage();
    }

    void EntityManager::_ApiLoad(Tools::Lua::CallHelper& helper)
    {
        Uint32 entityId = helper.PopArg("Server.Entity.Load: Missing argument \"target\"").To<Uint32>();
        auto it = this->_entities.find(entityId);
        if (it == this->_entities.end() || !it->second)
        {
            Tools::error << "EntityManager::_ApiLoad: Entity " << entityId << " not found, cannot load." << std::endl;
            return;
        }
        it->second->LoadFromStorage();
    }

    void EntityManager::_ApiKill(Tools::Lua::CallHelper& helper)
    {
        Uint32 entityId = helper.PopArg("Server.Entity.Kill: Missing argument \"target\"").Check<Uint32>("Server.Entity.Kill: Argument \"target\" must be a number");
        Tools::Lua::Ref arg(this->_engine.GetInterpreter().GetState());
        Uint32 cbTargetId = 0;
        std::string cbFunction;
        Tools::Lua::Ref cbArg(this->_engine.GetInterpreter().GetState());
        if (helper.GetNbArgs())
        {
            arg = helper.PopArg();
            if (helper.GetNbArgs())
            {
                cbTargetId = helper.PopArg().Check<Uint32>("Server.Entity.Kill: Argument \"cbTarget\" must be a number");
                cbFunction = helper.PopArg("Server.Entity.Kill: Missing argument \"cbFunction\"").CheckString("Server.Entity.Kill: Argument \"cbFunction\" must be a string");
                if (helper.GetNbArgs())
                    cbArg = helper.PopArg();
            }
        }
        Uint32 callbackId = 0;
        if (cbTargetId)
            callbackId = this->_engine.GetCallbackManager().MakeCallback(cbTargetId, cbFunction, cbArg);
        this->AddKillEvent(entityId, arg, callbackId);
    }

    void EntityManager::_ApiRegister(Tools::Lua::CallHelper& helper)
    {
        Uint32 pluginId = this->_engine.GetCurrentPluginRegistering();
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
            if (!Common::FieldUtils::IsRegistrableType(entityName = prototype["entityName"].ToString()))
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

    void EntityManager::_ApiGetWeakPointer(Tools::Lua::CallHelper& helper)
    {
        Entity const& e = this->GetEntity(helper.PopArg("Server.Entity.GetWeakPointer: Missing argument \"entity\""));
    }

    void EntityManager::_ApiRegisterPositional(Tools::Lua::CallHelper& helper)
    {
        helper.GetArgList().push_back(this->_engine.GetInterpreter().MakeBoolean(true));
        this->_ApiRegister(helper);
    }

    void EntityManager::_ApiSetPos(Tools::Lua::CallHelper& helper)
    {
//        Uint32 entityId = helper.PopArg("Server.Entity.SetPos: Missing argument \"target\"").Check<Uint32>("Server.Entity.SetPos: Argument \"target\" must be a number");
//        Common::Position pos = helper.PopArg("Server.Entity.SetPos: Missing argument \"position\"").Check<Common::Position>();
//        auto it = this->_positionalEntities.find(entityId);
//        if (it == this->_positionalEntities.end() || !it->second)
//        {
//            Tools::error << "EntityManager::_ApiSetPos: Positional entity " << entityId << " not found." << std::endl;
//            return;
//        }
//        it->second->SetPosition(pos);
//        this->_engine.GetDoodadManager().EntityHasMoved(entityId);
    }

    void EntityManager::_ApiGetPos(Tools::Lua::CallHelper& helper)
    {
//        Uint32 entityId = helper.PopArg("Server.Entity.GetPos: Missing argument \"target\"").Check<Uint32>("Server.Entity.GetPos: Argument \"target\" must be a number");
//        auto it = this->_positionalEntities.find(entityId);
//        if (it == this->_positionalEntities.end() || !it->second)
//        {
//            Tools::error << "EntityManager::_ApiGetPos: Positional entity " << entityId << " not found." << std::endl;
//            return; // retourne nil
//        }
//        helper.PushRet(it->second->GetPosition());
    }

    void EntityManager::_ApiSetSpeed(Tools::Lua::CallHelper& helper)
    {
//        Uint32 entityId = helper.PopArg("Server.Entity.SetPos: Missing argument \"target\"").Check<Uint32>("Server.Entity.SetPos: Argument \"target\" must be a number");
//        glm::dvec3 speed = helper.PopArg("Server.Entity.SetPos: Missing argument \"position\"").Check<glm::dvec3>();
//        auto it = this->_positionalEntities.find(entityId);
//        if (it == this->_positionalEntities.end() || !it->second)
//        {
//            Tools::error << "EntityManager::_ApiSetPos: Positional entity " << entityId << " not found." << std::endl;
//            return;
//        }
//        it->second->SetSpeed(speed);
//        this->_engine.GetDoodadManager().EntityHasMoved(entityId);
    }

    void EntityManager::_ApiGetSpeed(Tools::Lua::CallHelper& helper)
    {
//        Uint32 entityId = helper.PopArg("Server.Entity.GetPos: Missing argument \"target\"").Check<Uint32>("Server.Entity.GetPos: Argument \"target\" must be a number");
//        auto it = this->_positionalEntities.find(entityId);
//        if (it == this->_positionalEntities.end() || !it->second)
//        {
//            Tools::error << "EntityManager::_ApiGetPos: Positional entity " << entityId << " not found." << std::endl;
//            return; // retourne nil
//        }
//        helper.PushRet(it->second->GetSpeed());
    }

    void EntityManager::_ApiSetAccel(Tools::Lua::CallHelper& helper)
    {
//        Uint32 entityId = helper.PopArg("Server.Entity.SetPos: Missing argument \"target\"").Check<Uint32>("Server.Entity.SetPos: Argument \"target\" must be a number");
//        glm::dvec3 accel = helper.PopArg("Server.Entity.SetPos: Missing argument \"position\"").Check<glm::dvec3>();
//        auto it = this->_positionalEntities.find(entityId);
//        if (it == this->_positionalEntities.end() || !it->second)
//        {
//            Tools::error << "EntityManager::_ApiSetPos: Positional entity " << entityId << " not found." << std::endl;
//            return;
//        }
//        it->second->SetAccel(accel);
//        this->_engine.GetDoodadManager().EntityHasMoved(entityId);
    }

    void EntityManager::_ApiGetAccel(Tools::Lua::CallHelper& helper)
    {
//        Uint32 entityId = helper.PopArg("Server.Entity.GetPos: Missing argument \"target\"").Check<Uint32>("Server.Entity.GetPos: Argument \"target\" must be a number");
//        auto it = this->_positionalEntities.find(entityId);
//        if (it == this->_positionalEntities.end() || !it->second)
//        {
//            Tools::error << "EntityManager::_ApiGetPos: Positional entity " << entityId << " not found." << std::endl;
//            return; // retourne nil
//        }
//        helper.PushRet(it->second->GetAccel());
    }

}}}
