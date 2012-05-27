#include "server/game/engine/DoodadManager.hpp"
#include "server/game/engine/Engine.hpp"
#include "server/game/engine/Doodad.hpp"
#include "tools/lua/Interpreter.hpp"

namespace Server { namespace Game { namespace Engine {

    DoodadManager::DoodadManager(Engine& engine) :
        _engine(engine),
        _nextDoodadId(1) // le premier doodad sera le 1, 0 est la valeur spéciale "pas de doodad"
    {
        Tools::debug << "DoodadManager::DoodadManager()\n";
        auto& i = this->_engine.GetInterpreter();
        auto namespaceTable = i.Globals()["Server"].Set("Doodad", i.MakeTable());
        namespaceTable.Set("Spawn", i.MakeFunction(std::bind(&DoodadManager::_ApiSpawn, this, std::placeholders::_1)));
        namespaceTable.Set("Kill", i.MakeFunction(std::bind(&DoodadManager::_ApiKill, this, std::placeholders::_1)));
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
    }

    void DoodadManager::Save(Tools::Database::IConnection& conn)
    {
        // TODO
    }

    void DoodadManager::Load(Tools::Database::IConnection& conn)
    {
        // TODO
    }

    void DoodadManager::DeleteDoodadsOfEntity(Uint32 entityId)
    {
        auto listIt = this->_doodadsByEntities.find(entityId);
        if (listIt == this->_doodadsByEntities.end())
            return; // l'entité n'a aucun doodad
        auto it = listIt->second.begin();
        auto itEnd = listIt->second.end();
        for (; it != itEnd; ++it)
        {
            this->_doodads.erase((*it)->GetId());
            Tools::Delete(*it);
        }
        this->_doodadsByEntities.erase(listIt);
    }

    void DoodadManager::DisableDoodadsOfEntity(Uint32 entityId)
    {
        auto listIt = this->_doodadsByEntities.find(entityId);
        if (listIt == this->_doodadsByEntities.end())
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
        this->_doodadsByEntities.erase(listIt);
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
        assert(!this->_doodadsByEntities.count(entityId) && "une liste de doodads désactivés n'a pas été enlevée de la map de liste de doodads rangés par entités");
        this->_doodadsByEntities[entityId] = listIt->second;
        this->_disabledDoodads.erase(listIt);
    }

    void DoodadManager::_CreateDoodad(Uint32 doodadId, Uint32 pluginId, std::string const& name, Uint32 entityId, PositionalEntity const& entity)
    {
        if (this->_doodads.count(doodadId))
            throw std::runtime_error("a doodad with id " + Tools::ToString(doodadId) + " already exists");

        Doodad* d = new Doodad(doodadId, pluginId, name, entityId, entity, this->_engine.GetInterpreter());
        this->_doodads[doodadId] = d;
        this->_doodadsByEntities[entityId].push_back(d);
    }

    void DoodadManager::_ApiSpawn(Tools::Lua::CallHelper& helper)
    {
        Uint32 pluginId = this->_engine.GetRunningPluginId();
        if (!pluginId)
            throw std::runtime_error("Server.Doodad.Spawn: Could not determine currently running plugin, cannot spawn doodad");
        Uint32 entityId = this->_engine.GetEntityManager().GetRunningEntityId();
        if (helper.GetNbArgs() >= 2)
            entityId = static_cast<Uint32>(helper.PopArg().CheckNumber("Server.Doodad.Spawn: Argument \"entityId\" must be a number"));
        std::string doodadName = helper.PopArg("Server.Doodad.Spawn: Missing argument \"doodadName\"").CheckString("Server.Doodad.Spawn: Argument \"doodadName\" must be a string");
        if (!entityId || !this->_engine.GetEntityManager().IsEntityPositional(entityId))
        {
            Tools::error << "DoodadManager::_ApiSpawn: No positional entity with id " << entityId << ", cannot spawn doodad." << std::endl;
            return; // retourne nil
        }

        // trouve le prochain doodadId
        while (!this->_nextDoodadId // 0 est la valeur spéciale "pas de doodad", on la saute
                || this->_doodads.count(this->_nextDoodadId))
            ++this->_nextDoodadId;
        Uint32 newId = this->_nextDoodadId++;

        this->_CreateDoodad(newId, pluginId, doodadName, entityId, this->_engine.GetEntityManager().GetPositionalEntity(entityId));
        helper.PushRet(this->_engine.GetInterpreter().MakeNumber(newId));
    }

    void DoodadManager::_ApiKill(Tools::Lua::CallHelper& helper)
    {
        // trouve le doodad
        Uint32 doodadId = static_cast<Uint32>(helper.PopArg("Server.Doodad.Kill: Missing argument \"doodadId\"").CheckNumber("Server.Doodad.Kill: Argument \"doodadId\" must be a number"));
        auto it = this->_doodads.find(doodadId);
        if (it == this->_doodads.end())
        {
            Tools::error << "DoodadManager::_ApiKill: No doodad with id " << doodadId << ", cannot kill doodad." << std::endl;
            return;
        }

        // enleve le doodad de la liste associée à l'entité
        auto itList = this->_doodadsByEntities.find(it->second->GetEntityId());
        assert(itList != this->_doodadsByEntities.end() && "un doodad de la map générale des doodads n'était pas dans une liste associée a une entité");
        itList->second.remove(it->second);
        if (itList->second.empty())
            this->_doodadsByEntities.erase(itList); // supprime l'entrée associée à l'entité si celle-ci n'a plus de doodads

        // enleve le doodad de la map générales + delete
        Tools::Delete(it->second);
        this->_doodads.erase(it);
    }

}}}
