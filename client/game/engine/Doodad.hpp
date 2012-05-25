#ifndef __CLIENT_GAME_ENGINE_DOODAD_HPP__
#define __CLIENT_GAME_ENGINE_DOODAD_HPP__

#include "client/game/engine/DoodadType.hpp"
#include "tools/lua/Ref.hpp"

namespace Client { namespace Game { namespace Engine {

    class Doodad :
        private boost::noncopyable
    {
    private:
        DoodadType* _type;
        Tools::Lua::Ref _self;

    public:
        Doodad(Tools::Lua::Interpreter& interpreter, Uint32 id, DoodadType* type);
        DoodadType const& GetType() const { return *this->_type; }
        Tools::Lua::Ref const& GetSelf() const { return this->_self; }
    };

}}}

#endif
