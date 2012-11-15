#include "server/Server.hpp"
#include "server/Settings.hpp"
#include "server/game/engine/DoodadManager.hpp"
#include "server/game/engine/BodyManager.hpp"
#include "server/game/engine/Engine.hpp"
#include "server/game/engine/Doodad.hpp"
#include "server/game/engine/PositionalEntity.hpp"
#include "server/game/World.hpp"
#include "server/game/Game.hpp"
#include "server/game/PluginManager.hpp"
#include "server/game/map/Map.hpp"
#include "common/FieldUtils.hpp"
#include "tools/lua/Interpreter.hpp"
#include "tools/database/IConnection.hpp"

namespace Server { namespace Game { namespace Engine {

    DoodadManager::DoodadManager(Engine& engine) :
        _engine(engine),
        _nextDoodadId(1) // le premier doodad sera le 1, 0 est la valeur spéciale "pas de doodad"
    {
        Tools::debug << "DoodadManager::DoodadManager()\n";
        auto& i = this->_engine.GetInterpreter();

        this->_weakDoodadRefManager = new Tools::Lua::WeakResourceRefManager<WeakDoodadRef, DoodadManager>(
                i, /* interpreter */
                *this, /* resource manager */
                this->_engine.GetWorld().GetGame().GetServer().GetSettings().debug /* use fake reference */);

        auto namespaceTable = i.Globals()["Server"].Set("Doodad", i.MakeTable());

        namespaceTable.Set("Spawn", i.MakeFunction(std::bind(&DoodadManager::_ApiSpawn, this, std::placeholders::_1)));
        namespaceTable.Set("Kill", i.MakeFunction(std::bind(&DoodadManager::_ApiKill, this, std::placeholders::_1)));
        namespaceTable.Set("Set", i.MakeFunction(std::bind(&DoodadManager::_ApiSet, this, std::placeholders::_1)));
        namespaceTable.Set("Call", i.MakeFunction(std::bind(&DoodadManager::_ApiCall, this, std::placeholders::_1)));
        namespaceTable.Set("SetUdp", i.MakeFunction(std::bind(&DoodadManager::_ApiSetUdp, this, std::placeholders::_1)));
        namespaceTable.Set("CallUdp", i.MakeFunction(std::bind(&DoodadManager::_ApiCallUdp, this, std::placeholders::_1)));
        namespaceTable.Set("GetDoodadById", i.MakeFunction(std::bind(&DoodadManager::_ApiGetDoodadById, this, std::placeholders::_1)));
        namespaceTable.Set("GetWeakPointer", i.MakeFunction(std::bind(&DoodadManager::_ApiGetDoodadById, this, std::placeholders::_1)));

        namespaceTable.Set("SetAccel", i.MakeFunction(std::bind(&DoodadManager::_ApiSetAccel, this, std::placeholders::_1)));
        namespaceTable.Set("SetAcceleration", i.MakeFunction(std::bind(&DoodadManager::_ApiSetAccel, this, std::placeholders::_1)));
        namespaceTable.Set("SetLocalAccel", i.MakeFunction(std::bind(&DoodadManager::_ApiSetLocalAccel, this, std::placeholders::_1)));
        namespaceTable.Set("SetLocalAcceleration", i.MakeFunction(std::bind(&DoodadManager::_ApiSetLocalAccel, this, std::placeholders::_1)));
    }

    DoodadManager::~DoodadManager()
    {
        Tools::debug << "DoodadManager::~DoodadManager()\n";
        // doodads
        auto itDoodad = this->_doodads.begin();
        auto itDoodadEnd = this->_doodads.end();
        for (; itDoodad != itDoodadEnd; ++itDoodad)
            Tools::Delete(itDoodad->second);
        // disabled doodads
        auto itDisabledDoodad = this->_disabledDoodads.begin();
        auto itDisabledDoodadEnd = this->_disabledDoodads.begin();
        for (; itDisabledDoodad != itDisabledDoodadEnd; ++itDisabledDoodad)
        {
            auto it = itDisabledDoodad->second.begin();
            auto itEnd = itDisabledDoodad->second.end();
            for (; it != itEnd; ++it)
                Tools::Delete(*it);
        }
        // resource manager
        Tools::Delete(this->_weakDoodadRefManager);
    }

    Tools::Lua::Ref DoodadManager::WeakDoodadRef::GetReference(DoodadManager& doodadManager) const
    {
        return doodadManager.GetLuaWrapperForDoodad(this->doodadId);
    }

    std::string DoodadManager::WeakDoodadRef::Serialize(DoodadManager const&) const
    {
        return "return Server.Doodad.GetWeakPointer(" + Tools::ToString(this->doodadId) + ")";
    }

    bool DoodadManager::WeakDoodadRef::operator <(WeakDoodadRef const& rhs) const
    {
        if (this->disabled && !rhs.disabled)
            return true;
        else if (!this->disabled && rhs.disabled)
            return false;
        else
            return this->doodadId < rhs.doodadId;
    }

    void DoodadManager::Save(Tools::Database::IConnection& conn)
    {
        std::string table = this->_engine.GetMap().GetName() + "_doodad";
        conn.CreateQuery("DELETE FROM " + table)->ExecuteNonSelect();
        auto query = conn.CreateQuery("INSERT INTO " + table + " (id, plugin_id, doodad_name, entity_id, storage) VALUES (?, ?, ?, ?, ?);");

        // enabled doodads
        {
            auto it = this->_doodads.begin();
            auto itEnd = this->_doodads.end();
            for (; it != itEnd; ++it)
            {
                Doodad* d = it->second;
                if (!d)
                    continue;
                assert(d->GetId() == it->first && "lol");
                try
                {
                    std::string storage = this->_engine.GetInterpreter().GetSerializer().Serialize(d->GetStorage(), true /* nilOnError */);
                    Tools::debug << ">> Save >> " << table << " >> Enabled Doodad (id: " << d->GetId() << ", pluginId: " << d->GetPluginId() << ", doodadName: \"" << d->GetName() << "\", entityId: " << d->GetEntityId() << ", storage: " << storage.size() << " bytes)" << std::endl;
                    query->Bind(d->GetId()).Bind(d->GetPluginId()).Bind(d->GetName()).Bind(d->GetEntityId()).Bind(storage).ExecuteNonSelect().Reset();
                }
                catch (std::exception& e)
                {
                    Tools::error << "DoodadManager::Save: Could not save enabled doodad " << d->GetId() << ": " << e.what() << std::endl;
                }
            }
        }

        // disabled doodads
        {
            auto it = this->_disabledDoodads.begin();
            auto itEnd = this->_disabledDoodads.end();
            for (; it != itEnd; ++it)
            {
                auto itList = it->second.begin();
                auto itListEnd = it->second.end();
                for (; itList != itListEnd; ++itList)
                {
                    Doodad* d = *itList;
                    try
                    {
                        std::string storage = this->_engine.GetInterpreter().GetSerializer().Serialize(d->GetStorage(), true /* nilOnError */);
                        Tools::debug << ">> Save >> " << table << " >> Disabled Doodad (id: " << d->GetId() << ", pluginId: " << d->GetPluginId() << ", doodadName: \"" << d->GetName() << "\", entityId: " << d->GetEntityId() << ", storage: " << storage.size() << " bytes)" << std::endl;
                        query->Bind(d->GetId()).Bind(d->GetPluginId()).Bind(d->GetName()).Bind(d->GetEntityId()).Bind(storage).ExecuteNonSelect().Reset();
                    }
                    catch (std::exception& e)
                    {
                        Tools::error << "DoodadManager::Save: Could not save disabled doodad " << d->GetId() << ": " << e.what() << std::endl;
                    }
                }
            }
        }
    }

    void DoodadManager::Load(Tools::Database::IConnection& conn)
    {
        Uint32 maxId = 0;
        std::string table = this->_engine.GetMap().GetName() + "_doodad";
        auto query = conn.CreateQuery("SELECT id, plugin_id, doodad_name, entity_id, storage FROM " + table);
        while (auto row = query->Fetch())
        {
            Uint32 doodadId = row->GetUint32(0);
            if (doodadId > maxId)
                maxId = doodadId;
            Uint32 pluginId = row->GetUint32(1);
            std::string name = row->GetString(2);
            Uint32 entityId = row->GetUint32(3);
            try
            {
                Tools::Lua::Ref storage = this->_engine.GetInterpreter().GetSerializer().Deserialize(row->GetString(4));
                Tools::debug << "<< Load << " << table << " << Doodad (id: " << doodadId << ", pluginId: " << pluginId << ", doodadName: \"" << name << "\", entityId: " << entityId << ", storage: <lua>)" << std::endl;
                if (!storage.IsTable())
                    throw std::runtime_error("Storage must be of type table and not " + storage.GetTypeName());
                PositionalEntity* entity;
                try
                {
                    entity = &this->_engine.GetEntityManager().GetPositionalEntity(entityId);
                }
                catch (std::exception&)
                {
                    try
                    {
                        entity = &this->_engine.GetEntityManager().GetDisabledEntity(entityId);
                    }
                    catch (std::exception&)
                    {
                        throw std::runtime_error("No positional entity or disabled entity with id " + Tools::ToString(entityId));
                    }
                }
                this->_CreateDoodad(doodadId, pluginId, name, entityId, *entity, "")->SetStorage(storage); // TODO
            }
            catch (std::exception& e)
            {
                Tools::error << "DoodadManager::Load: Could not load doodad " << doodadId << ": " << e.what() << std::endl;
            }
        }
        this->_nextDoodadId = maxId + 1; // utile si jamais un script Lua fait des if sur des id de doodads

        // désactive les doodads des entités désactivées
        auto it = this->_engine.GetEntityManager().GetDisabledEntities().begin();
        auto itEnd = this->_engine.GetEntityManager().GetDisabledEntities().end();
        for (; it != itEnd; ++it)
            this->DisableDoodadsOfEntity(it->first);
    }

    void DoodadManager::ExecuteCommands()
    {
        auto it = this->_dirtyDoodads.begin();
        auto itEnd = this->_dirtyDoodads.end();
        for (; it != itEnd; ++it)
            (*it)->ExecuteCommands();
        this->_dirtyDoodads.clear();
    }

//    void DoodadManager::DoodadRemovedForPlayer(Uint32 playerId, Uint32 doodadId)
//    {
//        auto it = this->_doodads.find(doodadId);
//        if (it != this->_doodads.end() && it->second)
//            it->second->RemovePlayer(playerId);
//    }

    void DoodadManager::DeleteDoodadsOfEntity(Uint32 entityId)
    {
        auto listIt = this->_doodadsByEntity.find(entityId);
        if (listIt == this->_doodadsByEntity.end())
            return; // l'entité n'a aucun doodad
        auto it = listIt->second.begin();
        auto itEnd = listIt->second.end();
        for (; it != itEnd; ++it)
        {
            this->_doodads.erase((*it)->GetId());
            Tools::Delete(*it);
        }
        this->_doodadsByEntity.erase(listIt);
    }

    void DoodadManager::DisableDoodadsOfEntity(Uint32 entityId)
    {
        auto listIt = this->_doodadsByEntity.find(entityId);
        if (listIt == this->_doodadsByEntity.end())
            return; // l'entité n'a aucun doodad
        auto it = listIt->second.begin();
        auto itEnd = listIt->second.end();
        for (; it != itEnd; ++it)
        {
            (*it)->Disable();
            auto doodad = this->_doodads.find((*it)->GetId());
            assert(doodad != this->_doodads.end() && doodad->second && "un doodad n'est pas dans la map générale des doodads");
            doodad->second = 0;
        }
        assert(!this->_disabledDoodads.count(entityId) && "une liste de doodads n'a pas été enlevée de la map de liste rangés par entités");
        this->_disabledDoodads[entityId] = listIt->second;
        this->_doodadsByEntity.erase(listIt);
    }

    void DoodadManager::EnableDoodadsOfEntity(Uint32 entityId)
    {
        auto listIt = this->_disabledDoodads.find(entityId);
        if (listIt == this->_disabledDoodads.end())
            return; // aucun doodad désactivé pour cette entité
        auto it = listIt->second.begin();
        auto itEnd = listIt->second.end();
        for (; it != itEnd; ++it)
        {
            auto doodad = this->_doodads.find((*it)->GetId());
            assert(doodad != this->_doodads.end() && !doodad->second && "un doodad désactivé n'avait pas de pointeur nul dans la map générale des doodads");
            doodad->second = *it;
            (*it)->Enable();
        }
        assert(!this->_doodadsByEntity.count(entityId) && "une liste de doodads désactivés n'a pas été enlevée de la map de liste de doodads rangés par entités");
        this->_doodadsByEntity[entityId] = listIt->second;
        this->_disabledDoodads.erase(listIt);
    }

//    void DoodadManager::UpdatePhysicsFromDoodadOfEntity(Uint32 entityId)
//    {
//        auto listIt = this->_doodadsByEntity.find(entityId);
//        if (listIt == this->_doodadsByEntity.end())
//            return;
//
//        auto it = listIt->second.begin();
//        auto ite = listIt->second.end();
//        for (; it != ite; ++it)
//        {
//            auto doodad = this->_doodads.find((*it)->GetId());
//            assert(doodad != this->_doodads.end() && doodad->second && "un doodad n'est pas dans la map générale des doodads");
//            doodad->second->UpdatePhysics();
//        }
//    }

    bool DoodadManager::EntityHasDoodad(Uint32 entityId) const
    {
        return this->_doodadsByEntity.count(entityId) > 0;
    }

    //void DoodadManager::EntityHasMoved(Uint32 entityId)
    //{
    //    auto itList = this->_doodadsByEntity.find(entityId);
    //    if (itList == this->_doodadsByEntity.end())
    //        return;
    //    auto it = itList->second.begin();
    //    auto itEnd = itList->second.end();
    //    for (; it != itEnd; ++it)
    //        if ((*it))
    //            (*it)->PositionIsDirty();
    //}

    Tools::Lua::Ref DoodadManager::GetLuaWrapperForDoodad(Uint32 doodadId)
    {
        auto& i = this->_engine.GetInterpreter();
        auto object = i.MakeTable();
        object.Set("id", doodadId);
        object.Set("Set", i.MakeFunction(std::bind(&DoodadManager::_ApiSet, this, std::placeholders::_1)));
        object.Set("Call", i.MakeFunction(std::bind(&DoodadManager::_ApiCall, this, std::placeholders::_1)));
        object.Set("SetUdp", i.MakeFunction(std::bind(&DoodadManager::_ApiSetUdp, this, std::placeholders::_1)));
        object.Set("CallUdp", i.MakeFunction(std::bind(&DoodadManager::_ApiCallUdp, this, std::placeholders::_1)));
        /* TODO Kill + enlever throw */

        object.Set("SetAccel", i.MakeFunction(std::bind(&DoodadManager::_ApiSetAccel, this, std::placeholders::_1)));
        object.Set("SetAcceleration", i.MakeFunction(std::bind(&DoodadManager::_ApiSetAccel, this, std::placeholders::_1)));
        object.Set("SetLocalAccel", i.MakeFunction(std::bind(&DoodadManager::_ApiSetLocalAccel, this, std::placeholders::_1)));
        object.Set("SetLocalAcceleration", i.MakeFunction(std::bind(&DoodadManager::_ApiSetLocalAccel, this, std::placeholders::_1)));
        return object;
    }

    Doodad& DoodadManager::_GetDoodad(Uint32 doodadId) throw(std::runtime_error)
    {
        auto it = this->_doodads.find(doodadId);
        if (it == this->_doodads.end() || !it->second)
            throw std::runtime_error("DoodadManager::_GetDoodad: Doodad " + Tools::ToString(doodadId) + " not found.");
        return *it->second;
    }

    Uint32 DoodadManager::_RefToDoodadId(Tools::Lua::Ref const& ref) throw(std::runtime_error)
    {
        if (ref.IsNumber()) /* id directement en nombre */
            return ref.To<Uint32>();
        else if (ref.IsTable()) /* type final doodad wrapper (weak pointer déréférencé) */
            return ref["id"].Check<Uint32>("DoodadManager::_RefToDoodadId: Table argument has no id number field");
        else if (ref.IsUserData())
        {
            if (this->_weakDoodadRefManager->UsingFakeReferences() && ref.Is<Tools::Lua::WeakResourceRefManager<WeakDoodadRef, DoodadManager>::FakeReference*>()) /* doodad wrapper en fake reference (weak pointer déréférencé) */
            {
                auto fakeRef = ref.To<Tools::Lua::WeakResourceRefManager<WeakDoodadRef, DoodadManager>::FakeReference*>();
                if (fakeRef->IsValid())
                {
                    auto trueRef = fakeRef->GetReference();
                    if (trueRef.IsTable())
                        return trueRef["id"].Check<Uint32>("DoodadManager::_RefToDoodadId: Table argument has no id number field");
                    else
                        throw std::runtime_error("DoodadManager::_RefToDoodadId: This fake reference does not contain a table but a " + trueRef.GetTypeName() + ", is this really a doodad?");
                }
                else
                    throw std::runtime_error("DoodadManager::_RefToDoodadId: This reference was invalidated - you must not keep true references to doodads, only weak references");
            }
            else /* weak pointer (non déréférencé) */
            {
                WeakDoodadRef* d = ref.Check<WeakDoodadRef*>("DoodadManager::_RefToDoodadId: Userdata argument is not of WeakDoodadRef type");
                if (d->IsValid(*this))
                    return d->doodadId;
                else
                    throw std::runtime_error("DoodadManager::_RefToDoodadId: This weak pointer was invalidated - the doodad does not exist anymore");
            }
        }
        else
            throw std::runtime_error("DoodadManager::_RefToDoodadId: Invalid argument type " + ref.GetTypeName() + " given");
    }

    Doodad* DoodadManager::_CreateDoodad(Uint32 doodadId, Uint32 pluginId, std::string const& name, Uint32 entityId, PositionalEntity& entity, std::string const& bodyName)
    {
        if (this->_doodads.count(doodadId))
            throw std::runtime_error("a doodad with id " + Tools::ToString(doodadId) + " already exists");

        Body* body = 0;
        if (bodyName != "")
        {
            std::string bodyName0 = Common::FieldUtils::GetResourceNameFromResource(bodyName);
            std::string bodyPluginName = Common::FieldUtils::GetPluginNameFromResource(bodyName);
            Uint32 bodyPluginId = this->_engine.GetWorld().GetPluginManager().GetPluginId(bodyPluginName);
            body = this->_engine.GetBodyManager().CreateBody(bodyPluginId, bodyName0, entity.GetBodyCluster());
        }

        Doodad* d = new Doodad(
                this->_engine,
                doodadId,
                pluginId,
                name,
                entityId,
                entity,
                body
                );
        this->_doodads[doodadId] = d;
        this->_doodadsByEntity[entityId].push_back(d);
        return d;
    }

    void DoodadManager::_ApiSpawn(Tools::Lua::CallHelper& helper)
    {
        Uint32 entityId = helper.PopArg("Server.Doodad.Spawn: Missing argument \"entityId\"").Check<Uint32>("Server.Doodad.Spawn: Argument \"entityId\" must be a number");
        std::string doodadName = helper.PopArg("Server.Doodad.Spawn: Missing argument \"doodadName\"").CheckString("Server.Doodad.Spawn: Argument \"doodadName\" must be a string");
        std::string bodyName = helper.PopArg("Server.Doodad.Spawn: Missing argument \"bodyName\"").CheckString("Server.Doodad.Spawn: Argument \"bodyName\" must be a string");
        if (!this->_engine.GetEntityManager().IsEntityPositional(entityId))
        {
            Tools::error << "DoodadManager::_ApiSpawn: No positional entity with id " << entityId << ", cannot spawn doodad." << std::endl;
            return; // retourne nil
        }
        Uint32 pluginId = this->_engine.GetWorld().GetPluginManager().GetPluginId(Common::FieldUtils::GetPluginNameFromResource(doodadName));
        if (!pluginId)
            throw std::runtime_error("Server.Doodad.Spawn: Could not find the plugin associated with this doodad");

        // trouve le prochain doodadId
        while (!this->_nextDoodadId // 0 est la valeur spéciale "pas de doodad", on la saute
                || this->_doodads.count(this->_nextDoodadId))
            ++this->_nextDoodadId;
        Uint32 newId = this->_nextDoodadId++;

        Doodad* d = this->_CreateDoodad(newId, pluginId, doodadName, entityId, this->_engine.GetEntityManager().GetPositionalEntity(entityId), bodyName);
        helper.PushRet(this->_weakDoodadRefManager->GetWeakReference(d->GetWeakReferenceId()));

        // XXX test
        //auto const& players = this->_engine.GetMap().GetPlayers();
        //auto it = players.begin();
        //auto itEnd = players.end();
        //for (; it != itEnd; ++it)
        //    d->AddPlayer(it->first);
    }

    void DoodadManager::_ApiKill(Tools::Lua::CallHelper& helper)
    {
        // trouve le doodad
        Uint32 doodadId = helper.PopArg("Server.Doodad.Kill: Missing argument \"doodadId\"").Check<Uint32>("Server.Doodad.Kill: Argument \"doodadId\" must be a number");
        auto it = this->_doodads.find(doodadId);
        if (it == this->_doodads.end())
        {
            Tools::error << "DoodadManager::_ApiKill: No doodad with id " << doodadId << ", cannot kill doodad." << std::endl;
            return;
        }

        // enleve le doodad de la liste associée à l'entité
        auto itList = this->_doodadsByEntity.find(it->second->GetEntityId());
        assert(itList != this->_doodadsByEntity.end() && "un doodad de la map générale des doodads n'était pas dans une liste associée a une entité");
        itList->second.remove(it->second);
        if (itList->second.empty())
            this->_doodadsByEntity.erase(itList); // supprime l'entrée associée à l'entité si celle-ci n'a plus de doodads

        // enleve le doodad de la map générale + delete
        Tools::Delete(it->second);
        this->_doodads.erase(it);
    }

    void DoodadManager::_ApiSet(Tools::Lua::CallHelper& helper)
    {
        Doodad& d = this->_GetDoodad(this->_RefToDoodadId(helper.PopArg("Server.Doodad.Set: Missing argument \"doodad\"")));
        Tools::Lua::Ref key = helper.PopArg("Server.Doodad.Set: Missing argument \"key\"");
        Tools::Lua::Ref value = helper.PopArg("Server.Doodad.Set: Missing argument \"value\"");
        d.Set(key, value);
    }

    void DoodadManager::_ApiCall(Tools::Lua::CallHelper& helper)
    {
        Doodad& d = this->_GetDoodad(this->_RefToDoodadId(helper.PopArg("Server.Doodad.Call: Missing argument \"doodad\"")));
        std::string function = helper.PopArg("Server.Doodad.Call: Missing argument \"function\"").CheckString("Server.Doodad.Call: Argument \"function\" must be of type string");
        Tools::Lua::Ref value = helper.PopArg("Server.Doodad.Call: Missing argument \"value\"");
        d.Call(function, value);
    }

    void DoodadManager::_ApiSetUdp(Tools::Lua::CallHelper& helper)
    {
        Doodad& d = this->_GetDoodad(this->_RefToDoodadId(helper.PopArg("Server.Doodad.SetUdp: Missing argument \"doodad\"")));
        Tools::Lua::Ref key = helper.PopArg("Server.Doodad.SetUdp: Missing argument \"key\"");
        Tools::Lua::Ref value = helper.PopArg("Server.Doodad.SetUdp: Missing argument \"value\"");
        d.SetUdp(key, value);
    }

    void DoodadManager::_ApiCallUdp(Tools::Lua::CallHelper& helper)
    {
        Doodad& d = this->_GetDoodad(this->_RefToDoodadId(helper.PopArg("Server.Doodad.CallUdp: Missing argument \"doodad\"")));
        std::string function = helper.PopArg("Server.Doodad.CallUdp: Missing argument \"function\"").CheckString("Server.Doodad.CallUdp: Argument \"function\" must be of type string");
        Tools::Lua::Ref value = helper.PopArg("Server.Doodad.CallUdp: Missing argument \"value\"");
        d.CallUdp(function, value);
    }

    void DoodadManager::_ApiGetDoodadById(Tools::Lua::CallHelper& helper)
    {
        try
        {
            Doodad& d = this->_GetDoodad(this->_RefToDoodadId(helper.PopArg("Server.Doodad.CallUdp: Missing argument \"doodad\"")));
            helper.PushRet(this->_weakDoodadRefManager->GetWeakReference(d.GetWeakReferenceId()));
        }
        catch (std::exception& e)
        {
            Tools::error << "DoodadManager::_ApiGetDoodadById: Doodad not found (" << e.what() << "), invalid resource returned." << std::endl;
            helper.PushRet(this->_engine.GetInterpreter().MakeNil());
        }
    }

    void DoodadManager::_ApiGetWeakPointer(Tools::Lua::CallHelper& helper)
    {
        Tools::Lua::Ref doodadId = helper.PopArg("Server.Doodad.GetWeakPointer: Missing argument \"doodad\"");
        try
        {
            Doodad const& d = this->_GetDoodad(this->_RefToDoodadId(doodadId));
            helper.PushRet(this->_weakDoodadRefManager->GetWeakReference(d.GetWeakReferenceId()));
        }
        catch (std::exception&)
        {
            if (doodadId.IsNumber())
            {
                WeakDoodadRef unloadedDoodadRef(doodadId.To<Uint32>());
                helper.PushRet(this->_weakDoodadRefManager->NewUnloadedResource(unloadedDoodadRef));
            }
            else
                throw std::runtime_error("Server.Doodad.GetWeakPointer: Doodad not found - if you want to create a weak pointer to a non/maybe-existing doodad, use a number, not a " + doodadId.GetTypeName());
        }
    }

    void DoodadManager::_ApiSetAccel(Tools::Lua::CallHelper& helper)
    {
        Doodad& d = this->_GetDoodad(this->_RefToDoodadId(helper.PopArg("Server.Doodad.SetAccel: Missing argument \"doodad\"")));

        std::string node = helper.PopArg("Server.Doodad.SetAccel: Missing argument \"node\"").Check<std::string>("Server.Doodad.SetAccel: Argument \"node\" must be a string");
        glm::dvec3 accel = helper.PopArg("Server.Doodad.SetAccel: Missing argument \"accel\"").Check<glm::dvec3>("Server.Doodad.SetAccel: Argument \"accel\" must be a vector3");

        double maxSpeed = 8451;
        if (helper.GetNbArgs() > 0)
        {
            maxSpeed = helper.PopArg().Check<double>("Server.Doodad.SetAccel: Argument \"maxSpeed\" must be a double");
            if (maxSpeed <= 0)
                maxSpeed = 0.01;
        }

        d.SetAccel(node, accel, maxSpeed);
    }

    void DoodadManager::_ApiSetLocalAccel(Tools::Lua::CallHelper& helper)
    {
        Doodad& d = this->_GetDoodad(this->_RefToDoodadId(helper.PopArg("Server.Doodad.SetLocalAccel: Missing argument \"doodad\"")));

        std::string node = helper.PopArg("Server.Doodad.SetLocalAccel: Missing argument \"node\"").Check<std::string>("Server.Doodad.SetLocalAccel: Argument \"node\" must be a string");
        glm::dvec3 accel = helper.PopArg("Server.Doodad.SetLocalAccel: Missing argument \"accel\"").Check<glm::dvec3>("Server.Doodad.SetLocalAccel: Argument \"accel\" must be a vector3");

        double maxSpeed = 8451;
        if (helper.GetNbArgs() > 0)
        {
            maxSpeed = helper.PopArg().Check<double>("Server.Doodad.SetLocalAccel: Argument \"maxSpeed\" must be a double");
            if (maxSpeed <= 0)
                maxSpeed = 0.01;
        }

        d.SetLocalAccel(node, accel, maxSpeed);
    }

}}}
