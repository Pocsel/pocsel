#ifndef __TOOLS_LUA_RESOURCEMANAGER_HPP__
#define __TOOLS_LUA_RESOURCEMANAGER_HPP__

#include "tools/lua/Interpreter.hpp"

namespace Tools { namespace Lua {

    template<typename ResourceType, typename ManagerType> class ResourceManager :
        private boost::noncopyable
    {
    private:
        Interpreter& _interpreter;
        MetaTable& _metaTable;
        std::unordered_map<Uint32 /* resource id */, Ref /* UserData */> _resources;
        Uint32 _nextResourceId;
        ManagerType& _resourceManager;

    public:
        ResourceManager(Interpreter& interpreter, ManagerType& resourceManager) :
            _interpreter(interpreter),
            _metaTable(MetaTable::Create(interpreter, ResourceType())),
            _nextResourceId(1),
            _resourceManager(resourceManager)
        {
            this->_metaTable.SetMetaMethod(MetaTable::Call, std::bind(&ResourceManager::_MetaMethodCall, this, std::placeholders::_1));
            this->_metaTable.SetMetaMethod(MetaTable::Index, std::bind(&ResourceManager::_MetaMethodIndex, this, std::placeholders::_1));
            this->_metaTable.SetMetaMethod(MetaTable::NewIndex, std::bind(&ResourceManager::_MetaMethodNewIndex, this, std::placeholders::_1));
        }

        std::pair<Uint32, Ref> NewResource(ResourceType const& resource)
        {
            while (!this->_nextResourceId // 0 est la valeur spéciale "pas de resource", on la saute
                    || this->_resources.count(this->_nextResourceId))
                ++this->_nextResourceId;
            Uint32 newId = this->_nextResourceId++;
            Ref userData = this->_interpreter.Make(resource);
            return *this->_resources.insert(std::make_pair(newId, userData)).first;
        }

        Ref GetUserDataForResource(Uint32 resourceId) throw(std::runtime_error)
        {
            auto it = this->_resources.find(resourceId);
            if (it == this->_resources.end())
                throw std::runtime_error("ResourceManager::GetUserDataForResource: No resource with id " + Tools::ToString(resourceId));
            return it->second;
        }

        void UpdateResource(Uint32 resourceId, ResourceType const& newResource) throw(std::runtime_error)
        {
            auto it = this->_resources.find(resourceId);
            if (it == this->_resources.end())
                throw std::runtime_error("ResourceManager::UpdateResource: No resource with id " + Tools::ToString(resourceId));
            Ref& ref = it->second;
            ResourceType* resource = ref.To<ResourceType*>(); // pourquoi ça ne marche qu'en deux étapes ?
            *resource = newResource;
        }

        void InvalidateResource(Uint32 resourceId) throw(std::runtime_error)
        {
            auto it = this->_resources.find(resourceId);
            if (it == this->_resources.end())
                throw std::runtime_error("ResourceManager::InvalidateResource: No resource with id " + Tools::ToString(resourceId));
            Ref& ref = it->second;
            ref.To<ResourceType*>()->Invalidate(); // pourquoi ça ne marche qu'en deux étapes ?
            this->_resources.erase(it);
        }

    private:
        void _MetaMethodCall(CallHelper& helper)
        {
            ResourceType* resource = helper.PopArg("Tools::Lua::ResourceManager: Metamethod __call with no argument").To<ResourceType*>();
            helper.PushRet(this->_interpreter.MakeBoolean(resource->IsValid()));
        }

        void _MetaMethodIndex(CallHelper& helper)
        {
            ResourceType* resource = helper.PopArg("Tools::Lua::ResourceManager: Metamethod __index with no argument").To<ResourceType*>();
            if (resource->IsValid())
                resource->Index(this->_resourceManager, helper);
            else
                throw std::runtime_error("Tools::Lua::ResourceManager: Metamethod __index called on an invalid resource - check it with operator () before");
        }

        void _MetaMethodNewIndex(CallHelper& helper)
        {
            ResourceType* resource = helper.PopArg("Tools::Lua::ResourceManager: Metamethod __newindex with no argument").To<ResourceType*>();
            if (resource->IsValid())
                resource->NewIndex(this->_resourceManager, helper);
            else
                throw std::runtime_error("Tools::Lua::ResourceManager: Metamethod __newindex called on an invalid resource - check it with operator () before");
        }
    };

}}

#endif
