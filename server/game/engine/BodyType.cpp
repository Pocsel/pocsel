#include "server/game/engine/BodyType.hpp"

namespace Server { namespace Game { namespace Engine {

    BodyType::BodyType(std::string const& name, Uint32 pluginId, Tools::Lua::Ref const& prototype) :
        _name(name), _pluginId(pluginId), _prototype(prototype)
    {
        if (prototype["shapeTree"].Exists())
        {
            if (!prototype["shapeTree"].IsTable())
                throw std::runtime_error("Client.Model.Register: Field \"shapeTree\" must be of type table");
            Tools::Lua::Ref shapeTree = prototype.GetTable("shapeTree");
            Tools::Lua::Iterator stIt = shapeTree.Begin();
            Tools::Lua::Iterator toto = shapeTree.Begin();
        }
    }

}}}
