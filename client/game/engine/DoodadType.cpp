#include "client/game/engine/DoodadType.hpp"

namespace Client { namespace Game { namespace Engine {

    DoodadType::DoodadType(Uint32 pluginId, std::string const& name, Tools::Lua::Ref const& prototype) :
        _pluginId(pluginId), _name(name), _prototype(prototype)
    {
    }

}}}
