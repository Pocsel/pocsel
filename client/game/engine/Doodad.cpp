#include "client/game/engine/Doodad.hpp"
#include "tools/lua/Interpreter.hpp"

namespace Client { namespace Game { namespace Engine {

    Doodad::Doodad(Tools::Lua::Interpreter& interpreter, Uint32 id, Common::Physics::Node const& physics, DoodadType const& type) :
        _type(type),
        _self(interpreter.MakeTable()),
        _physics(physics)
    {
        this->_self.Set("id", id);
        Tools::debug << "Doodad::Doodad: New doodad \"" << this->_type.GetName() << "\" spawned (id: " << id << ", pos: " << this->_physics.position.r.x << " " << this->_physics.position.r.y << " " << this->_physics.position.r.z << ")" << std::endl;
    }

    Doodad::~Doodad()
    {
        Tools::debug << "Doodad::~Doodad: Destroying doodad \"" << this->_type.GetName() << "\" (pos: " << this->_physics.position.r.x << " " << this->_physics.position.r.y << " " << this->_physics.position.r.z << ")" << std::endl;
    }

}}}
