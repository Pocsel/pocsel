#ifndef __CLIENT_GAME_ENGINE_DOODAD_HPP__
#define __CLIENT_GAME_ENGINE_DOODAD_HPP__

#include "client/game/engine/DoodadType.hpp"
#include "tools/lua/Ref.hpp"
#include "common/Position.hpp"

namespace Client { namespace Game { namespace Engine {

    class Doodad :
        private boost::noncopyable
    {
    private:
        DoodadType const& _type;
        Tools::Lua::Ref _self;
        Common::Position _pos;

    public:
        Doodad(Tools::Lua::Interpreter& interpreter, Uint32 id, Common::Position const& pos, DoodadType const& type);
        ~Doodad();
        DoodadType const& GetType() const { return this->_type; }
        Tools::Lua::Ref const& GetSelf() const { return this->_self; }
        Common::Position const& GetPosition() const { return this->_pos; }
        void SetPosition(Common::Position const& pos) { this->_pos = pos; }
    };

}}}

#endif
