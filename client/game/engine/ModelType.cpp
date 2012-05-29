#include "client/game/engine/ModelType.hpp"

#include "tools/lua/Ref.hpp"
#include "tools/lua/Function.hpp"

#include "client/resources/ResourceManager.hpp"

namespace Client { namespace Game { namespace Engine {

    ModelType::ModelType(Uint32 pluginId, std::string const& name, Tools::Lua::Ref& prototype, Resources::ResourceManager& resourceManager) :
        _pluginId(pluginId), _name(name)
    {
        if (!prototype["file"].IsString())
            throw std::runtime_error("Client.Model.Register: Field \"file\" must exist and be of type string");
        _resourceId = resourceManager.GetResourceId(pluginId, prototype["file"].ToString());
    }

}}}
