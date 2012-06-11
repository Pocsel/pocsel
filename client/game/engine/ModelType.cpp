#include "client/game/engine/ModelType.hpp"

#include "tools/lua/Ref.hpp"
#include "tools/lua/Function.hpp"

#include "client/resources/ResourceManager.hpp"

namespace Client { namespace Game { namespace Engine {

    ModelType::ModelType(std::string const& name, Uint32 resourceId) :
        _name(name),
        _resourceId(resourceId)
    {
        if (!prototype["file"].IsString())
            throw std::runtime_error("Client.Model.Register: Field \"file\" must exist and be of type string");
        _resourceId = resourceManager.GetResourceId(pluginId, prototype["file"].ToString());
    }

}}}
