#ifndef __TOOLS_LUA_RESOURCEMANAGER_HPP__
#define __TOOLS_LUA_RESOURCEMANAGER_HPP__

#include "tools/lua/Interpreter.hpp"

namespace Tools { namespace Lua {

    template <typename T>
    class ResourceManager :
        private boost::noncopyable
    {
    private:
        Interpreter& _interpreter;
        MetaTable& _metaTable;
        T _invalidResource;
        std::map<T /* resource */, Ref /* UserData */> _resources;

    public:
        ResourceManager(Interpreter& interpreter, T const& invalidResource = T()) :
            _interpreter(interpreter), _metaTable(MetaTable::Create(interpreter, T())), _invalidResource(invalidResource)
        {
            this->_metaTable.SetMetaMethod(MetaTable::Call, std::bind(&ResourceManager<T>::_OverloadedCall, this, std::placeholders::_1));
        }

        MetaTable& GetMetaTable()
        {
            return this->_metaTable;
        }

        Ref NewResource(T const& resource)
        {
            assert(!(resource == this->_invalidResource) && "ne pas créer de resource invalide");
            auto it = this->_resources.find(resource);
            if (it == this->_resources.end())
            {
                Ref userData = this->_interpreter.Make(resource);
                this->_resources.insert(std::make_pair(resource, userData));
                return userData;
            }
            return it->second;
        }

        void DeleteResource(T const& resource)
        {
            auto it = this->_resources.find(resource);
            assert(it != this->_resources.end() && "suppression d'une resource qui n'a pas ete créée");
            *reinterpret_cast<T*>(it->second.ToUserData()) = this->_invalidResource;
            this->_resources.erase(it);
        }

    private:
        void _OverloadedCall(CallHelper& helper)
        {
            T* resource = helper.PopArg("Tools::Lua::ResourceManager: Metamethod __call with no argument").To<T*>();
            if (*resource == this->_invalidResource)
                helper.PushRet(this->_interpreter.MakeBoolean(false));
            else
            {
                assert(this->_resources.find(*resource) != this->_resources.end() && "test de validité d'une resource qui n'est pas dans sa map (oubli de NewResource() probablement)");
                helper.PushRet(this->_interpreter.MakeBoolean(true));
            }
        }
    };

}}

#endif
