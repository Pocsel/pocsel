#include "server/game/engine/EntityType.hpp"

namespace Server { namespace Game { namespace Engine {

    EntityType::EntityType(std::string const& name, Uint32 pluginId, Luasel::Ref const& prototype, bool isPositional) :
        _name(name), _pluginId(pluginId), _prototype(prototype), _isPositional(isPositional)
    {
    }

}}}
