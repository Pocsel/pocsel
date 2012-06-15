#ifndef __TOOLS_LUA_WEAKREF_HPP__
#define __TOOLS_LUA_WEAKREF_HPP__

#include "tools/lua/Ref.hpp"
#include "tools/lua/State.hpp"

namespace Tools { namespace Lua {

    class WeakRef
    {
    private:
        State& _state;
        Uint32 _id;

    public:
        explicit WeakRef(Ref const& ref);
        ~WeakRef();

        WeakRef& operator =(Ref const& ref);
        WeakRef& operator =(WeakRef const& ref);

        Ref Lock() const;
    };

}}

#endif
