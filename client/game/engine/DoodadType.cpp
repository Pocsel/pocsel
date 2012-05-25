#include "client/game/engine/DoodadType.hpp"

namespace Client { namespace Game { namespace Engine {

    DoodadType::DoodadType(std::string const& name, Uint32 pluginId, Tools::Lua::Ref const& prototype) :
        _name(name), _pluginId(pluginId), _prototype(prototype)
    {
    }

}}}
