#ifndef __TOOLS_LUA_SETTERS_HPP__
#define __TOOLS_LUA_SETTERS_HPP__

#include "tools/lua/StackRefPtr.hpp"
#include "tools/lua/StackRef.hpp"

namespace Tools { namespace Lua {

    struct GlobalSetter
    {
    private:
        StackRefPtr _ref;
        std::string _name;

    public:
        GlobalSetter(StackRefPtr ref, std::string const& name) : _ref(ref), _name(name) {}
        void operator ()(Object&)
        {
            auto& api = _ref->luaState;
            api.getref(_ref->idx);
            api.setglobal(_name.c_str());
        }
    };

    struct ArraySetter
    {
    private:
        StackRefPtr _ref;
        int _idx;

    public:
        ArraySetter(StackRefPtr ref, int idx) : _ref(ref), _idx(idx) {}

        void operator() (Object& o)
        {
            auto& api = _ref->luaState;
            api.getref(_ref->idx);
            api.getref(o.GetIndex());
            api.rawseti(-2, _idx);
            api.pop(1);
        }

    };

    struct TableSetter
    {
    private:
        StackRefPtr _tableRef;
        StackRefPtr _keyRef;

    public:
        TableSetter(StackRefPtr tableRef, StackRefPtr keyRef) :
            _tableRef(tableRef),
            _keyRef(keyRef)
        {}

        void operator() (Object& o)
        {
            auto& api = _tableRef->luaState;
            api.getref(_tableRef->idx);
            api.getref(_keyRef->idx);
            api.getref(o.GetIndex());
            api.settable(-3);
            api.pop(1);
        }
    };

}}

#endif
