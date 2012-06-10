#include "client/game/engine/ModelType.hpp"

namespace Client { namespace Game { namespace Engine {

    ModelType::ModelType(std::string const& name, Uint32 resourceId) :
        _name(name),
        _resourceId(resourceId)
    {
    }

}}}
