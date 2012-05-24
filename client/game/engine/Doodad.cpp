#include "client/game/engine/Doodad.hpp"

namespace Client { namespace Game { namespace Engine {

    Doodad::Doodad(Tools::Lua::Interpreter& interpreter, Uint32 id, DoodadType* type) :
        _type(type), _self(interpreter.MakeTable())
    {
        this->_self.Set("id", id);
    }

}}}
