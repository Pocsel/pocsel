#ifndef __CLIENT_GAME_ENGINE_DOODADTYPE_HPP__
#define __CLIENT_GAME_ENGINE_DOODADTYPE_HPP__

#include "tools/lua/Ref.hpp"

namespace Client { namespace Game { namespace Engine {

    class DoodadType :
        private boost::noncopyable
    {
    private:
        std::string _name;
        Tools::Lua::Ref _prototype;

    public:
        DoodadType(std::string const& name, Tools::Lua::Ref const& prototype);
        Tools::Lua::Ref const& GetPrototype() const { return this->_prototype; }
        std::string const& GetName() const { return this->_name; }
    };

}}}

#endif
