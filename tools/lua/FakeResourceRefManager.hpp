#ifndef __TOOLS_LUA_FAKERESOURCEREFMANAGER_HPP__
#define __TOOLS_LUA_FAKERESOURCEREFMANAGER_HPP__

#include "tools/lua/Interpreter.hpp"

namespace Tools { namespace Lua {

    template<typename FakeResourceRefType, typename ManagerType> class FakeResourceRefManager :
        private boost::noncopyable
    {
    private:
        Interpreter& _interpreter;
        MetaTable& _metaTable;
        std::unordered_map<Uint32 /* resource id */, Ref /* UserData */> _resources;
        Uint32 _nextResourceId;
        ManagerType& _resourceManager;
        Uint32 _invalidResource;

    public:
        FakeResourceRefManager(Interpreter& interpreter, ManagerType& resourceManager, FakeResourceRefType const& invalidResource) :
            _interpreter(interpreter),
            _metaTable(MetaTable::Create(interpreter, FakeResourceRefType())),
            _nextResourceId(1),
            _resourceManager(resourceManager)
        {
            this->_metaTable.SetMetaMethod(MetaTable::Call, std::bind(&FakeResourceRefManager::_MetaMethodCall, this, std::placeholders::_1));
            this->_metaTable.SetMetaMethod(MetaTable::Index, std::bind(&FakeResourceRefManager::_MetaMethodIndex, this, std::placeholders::_1));
            this->_metaTable.SetMetaMethod(MetaTable::NewIndex, std::bind(&FakeResourceRefManager::_MetaMethodNewIndex, this, std::placeholders::_1));
            this->_invalidResource = this->NewResource(invalidResource).first;
        }

        Ref GetInvalidResourceReference() const
        {
            return this->_resources.find(this->_invalidResource)->second;
        }

        Uint32 GetInvalidResourceId() const
        {
            return this->_invalidResource;
        }

        std::pair<Uint32, Ref> NewResource(FakeResourceRefType const& resource)
        {
            while (!this->_nextResourceId // 0 est la valeur spéciale "pas de resource", on la saute
                    || this->_resources.count(this->_nextResourceId))
                ++this->_nextResourceId;
            Uint32 newId = this->_nextResourceId++;
            Ref userData = this->_interpreter.Make(resource);
            return *this->_resources.insert(std::make_pair(newId, userData)).first;
        }

        Ref GetResourceReference(Uint32 resourceId) throw(std::runtime_error)
        {
            auto it = this->_resources.find(resourceId);
            if (it == this->_resources.end())
                throw std::runtime_error("FakeResourceRefManager::GetResourceReference: No resource with id " + Tools::ToString(resourceId));
            return it->second;
        }

        // ne pas garder la reference longtemps...
        FakeResourceRefType& GetResource(Uint32 resourceId) throw(std::runtime_error)
        {
            auto it = this->_resources.find(resourceId);
            if (it == this->_resources.end())
                throw std::runtime_error("FakeResourceRefManager::GetResource: No resource with id " + Tools::ToString(resourceId));
            Ref& ref = it->second;
            return *ref.To<FakeResourceRefType*>();
        }

        void ReplaceResource(Uint32 resourceId, FakeResourceRefType const& newResource) throw(std::runtime_error)
        {
            auto it = this->_resources.find(resourceId);
            if (it == this->_resources.end())
                throw std::runtime_error("FakeResourceRefManager::ReplaceResource: No resource with id " + Tools::ToString(resourceId));
            Ref& ref = it->second;
            FakeResourceRefType* resource = ref.To<FakeResourceRefType*>(); // pourquoi ça ne marche qu'en deux étapes ?
            *resource = newResource;
        }

        void InvalidateResource(Uint32 resourceId) throw(std::runtime_error)
        {
            auto it = this->_resources.find(resourceId);
            if (it == this->_resources.end())
                throw std::runtime_error("FakeResourceRefManager::InvalidateResource: No resource with id " + Tools::ToString(resourceId));
            Ref& ref = it->second;
            ref.To<FakeResourceRefType*>()->Invalidate(); // pourquoi ça ne marche qu'en deux étapes ?
            this->_resources.erase(it);
        }

        MetaTable& GetMetaTable()
        {
            return this->_metaTable;
        }

    private:
        void _MetaMethodCall(CallHelper& helper)
        {
            FakeResourceRefType* resource = helper.PopArg("Tools::Lua::FakeResourceRefManager: Metamethod __call with no argument").To<FakeResourceRefType*>();
            helper.PushRet(this->_interpreter.MakeBoolean(resource->IsValid()));
        }

        void _MetaMethodIndex(CallHelper& helper)
        {
            FakeResourceRefType* resource = helper.PopArg("Tools::Lua::FakeResourceRefManager: Metamethod __index with no argument").To<FakeResourceRefType*>();
            if (resource->IsValid())
                resource->Index(this->_resourceManager, helper);
            else
                throw std::runtime_error("Tools::Lua::FakeResourceRefManager: Metamethod __index called on an invalid resource - check it with operator () before");
        }

        void _MetaMethodNewIndex(CallHelper& helper)
        {
            FakeResourceRefType* resource = helper.PopArg("Tools::Lua::FakeResourceRefManager: Metamethod __newindex with no argument").To<FakeResourceRefType*>();
            if (resource->IsValid())
                resource->NewIndex(this->_resourceManager, helper);
            else
                throw std::runtime_error("Tools::Lua::FakeResourceRefManager: Metamethod __newindex called on an invalid resource - check it with operator () before");
        }
    };

}}

#endif
