#include "server/game/engine/Doodad.hpp"

namespace Server { namespace Game { namespace Engine {

    Doodad::Doodad(Uint32 id, Uint32 pluginId, std::string const& name, Uint32 entityId, PositionalEntity const& entity) :
        _id(id), _pluginId(pluginId), _name(name), _entityId(entityId), _entity(entity)
    {
    }

    void Doodad::Disable()
    {
    }

    void Doodad::Enable()
    {
    }

}}}
