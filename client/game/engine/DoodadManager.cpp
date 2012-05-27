#include "client/game/engine/DoodadManager.hpp"
#include "client/game/engine/Doodad.hpp"
#include "client/game/engine/DoodadType.hpp"

namespace Client { namespace Game { namespace Engine {

    DoodadManager::DoodadManager(Engine& engine) :
        _engine(engine)
    {
    }

    Uint32 DoodadManager::GetRunningPluginId() const
    {
        return this->_runningDoodad ? this->_runningDoodad->GetType().GetPluginId() : 0;
    }

    Doodad const& DoodadManager::GetDoodad(Uint32 doodadId) const throw(std::runtime_error)
    {
        throw std::runtime_error("bite");
        //auto it = this->_doodads.find(doodadId);
        //if (it == this->_doodads.end() || !it->second)
        //    throw std::runtime_error("DoodadManager: Doodad not found.");
        //return *it->second;
    }

    void DoodadManager::SpawnDoodad(Uint32 doodadId,
            Uint32 pluginId,
            std::string const& doodadName,
            Common::Position const& position,
            std::list<std::pair<std::string /* key */, std::string /* value */>> const& values)
    {

    }

    void DoodadManager::KillDoodad(Uint32 doodadId)
    {

    }

    void DoodadManager::UpdateDoodad(Uint32 doodadId,
            Common::Position const* position,
            std::list<std::tuple<bool /* functionCall */, std::string /* function || key */, std::string /* value */>> const& commands)
    {

    }

}}}
