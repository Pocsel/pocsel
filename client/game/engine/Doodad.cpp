#include "client/game/engine/Doodad.hpp"
#include "tools/lua/Interpreter.hpp"

namespace Client { namespace Game { namespace Engine {

    Doodad::Doodad(Tools::Lua::Interpreter& interpreter, Uint32 id, Common::Position const& pos, DoodadType const& type) :
        _type(type), _self(interpreter.MakeTable()), _pos(pos)
    {
        this->_self.Set("id", id);
        Tools::debug << "Doodad::Doodad: New doodad \"" << this->_type.GetName() << "\" spawned (id: " << id << ", pos: " << this->_pos.x << " " << this->_pos.y << " " << this->_pos.z << ")" << std::endl;
    }

    Doodad::~Doodad()
    {
        Tools::debug << "Doodad::~Doodad: Destroying doodad \"" << this->_type.GetName() << "\" (pos: " << this->_pos.x << " " << this->_pos.y << " " << this->_pos.z << ")" << std::endl;
    }

}}}
