#include "tools/precompiled.hpp"

#include "tools/lua/WeakRef.hpp"

namespace Tools { namespace Lua {

    WeakRef::WeakRef(Ref const& ref) :
        _state(ref.GetState()),
        _id(_state.GetWeakReference(ref))
    {
    }

    WeakRef::~WeakRef()
    {
    }

    WeakRef& WeakRef::operator =(Ref const& ref)
    {
        assert(&this->_state == &ref.GetState() && "Impossible de copier une référence d'un autre interpreteur");
        this->_id = _state.GetWeakReference(ref);
        return *this;
    }

    WeakRef& WeakRef::operator =(WeakRef const& ref)
    {
        if (this != &ref)
        {
            assert(&this->_state == &ref._state && "Impossible de copier une référence d'un autre interpreteur");
            this->_id = ref._id;
        }
        return *this;
    }

    Ref WeakRef::Lock() const
    {
        return this->_state.GetWeakReference(this->_id);
    }

}}