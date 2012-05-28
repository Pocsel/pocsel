#include "client/game/engine/ModelType.hpp"

namespace Client { namespace Game { namespace Engine {

    ModelType::ModelType(Uint32 pluginId, std::string const& name, Uint32 resourceId) :
        _pluginId(pluginId), _name(name), _resourceId(resourceId)
    {
    }

}}}
