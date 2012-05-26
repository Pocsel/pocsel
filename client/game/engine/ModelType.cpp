#include "client/game/engine/ModelType.hpp"

namespace Client { namespace Game { namespace Engine {

    ModelType::ModelType(std::string const& name, Uint32 pluginId, Tools::Lua::Ref const& prototype) :
        _name(name), _pluginId(pluginId), _prototype(prototype)
    {
    }

}}}
