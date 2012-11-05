#include "client/game/engine/DoodadType.hpp"

namespace Client { namespace Game { namespace Engine {

    DoodadType::DoodadType(std::string const& name, Luasel::Ref const& prototype) :
        _name(name),
        _prototype(prototype)
    {
    }

}}}
