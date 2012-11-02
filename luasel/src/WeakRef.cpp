#include <cassert>
#include <luasel/WeakRef.hpp>

namespace Luasel {

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
        assert(&this->_state == &ref.GetState() && "Cannot copy a reference from another interpreter");
        this->_id = _state.GetWeakReference(ref);
        return *this;
    }

    WeakRef& WeakRef::operator =(WeakRef const& ref)
    {
        if (this != &ref)
        {
            assert(&this->_state == &ref._state && "Cannot copy a reference from another interpreter");
            this->_id = ref._id;
        }
        return *this;
    }

    Ref WeakRef::Lock() const
    {
        return this->_state.GetWeakReference(this->_id);
    }

}
