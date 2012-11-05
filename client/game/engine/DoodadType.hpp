#ifndef __CLIENT_GAME_ENGINE_DOODADTYPE_HPP__
#define __CLIENT_GAME_ENGINE_DOODADTYPE_HPP__

#include <luasel/Luasel.hpp>

namespace Client { namespace Game { namespace Engine {

    class DoodadType :
        private boost::noncopyable
    {
    private:
        std::string _name;
        Luasel::Ref _prototype;

    public:
        DoodadType(std::string const& name, Luasel::Ref const& prototype);
        Luasel::Ref const& GetPrototype() const { return this->_prototype; }
        std::string const& GetName() const { return this->_name; }
    };

}}}

#endif
