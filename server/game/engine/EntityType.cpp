#include "server/game/engine/EntityType.hpp"

namespace Server { namespace Game { namespace Engine {

    EntityType(std::string const& name, Uint32 pluginId, Tools::Lua::Ref const& prototype) :
        _name(name), _pluginId(pluginId), _prototype(prototype)
    {
    }

}}}
