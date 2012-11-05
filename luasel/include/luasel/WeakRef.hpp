#ifndef __LUASEL_WEAKREF_HPP__
#define __LUASEL_WEAKREF_HPP__

#include <luasel/Ref.hpp>
#include <luasel/State.hpp>

namespace Luasel {

    class WeakRef
    {
        private:
            State& _state;
            unsigned int _id;

        public:
            explicit WeakRef(Ref const& ref);
            ~WeakRef();

            WeakRef& operator =(Ref const& ref);
            WeakRef& operator =(WeakRef const& ref);

            Ref Lock() const;
    };

}

#endif
