#ifndef __SERVER_GAME_ENGINE_BODY_HPP__
#define __SERVER_GAME_ENGINE_BODY_HPP__

#include "tools/lua/Ref.hpp"

namespace Server { namespace Game { namespace Engine {

    class Engine;
    class BodyType;

    class Body :
        private boost::noncopyable
    {
    private:
        BodyType const& _type;
        //Tools::Lua::Ref _table;
        bool _dirty;

    public:
        Body(Tools::Lua::Interpreter& interpreter, BodyType const& bodyType);
    };

}}}

#endif
