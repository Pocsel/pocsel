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

}}}
