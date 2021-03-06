#ifndef __TOOLS_LUA_WEAKRESOURCEREFMANAGER_HPP__
#define __TOOLS_LUA_WEAKRESOURCEREFMANAGER_HPP__

#include <luasel/Luasel.hpp>

namespace Tools { namespace Lua {

    template<typename WeakResourceRefType, typename ManagerType> class WeakResourceRefManager :
        private boost::noncopyable
    {
    public:
        struct FakeReference
        {
            FakeReference(Luasel::Ref const& ref) : trueReference(ref) {}
            bool IsValid() const { return this->trueReference.IsValid(); }
            void Invalidate() { this->trueReference.Unref(); }
            Luasel::Ref GetReference() const { return this->trueReference; }
            Luasel::Ref trueReference;
        };

    private:
        Luasel::Interpreter& _interpreter;
        ManagerType& _resourceManager;
        Uint32 _nextReferenceId;
        std::unordered_map<Uint32 /* ref id */, std::list<std::pair<WeakResourceRefType* /* pointer to Lua resource */, Luasel::Ref /* corresponding UserData */>>>* _weakReferences;
        std::map<WeakResourceRefType, std::pair<Uint32 /* ref id */, std::list<std::pair<WeakResourceRefType* /* pointer to Lua resource */, Luasel::Ref /* corresponding UserData */>>*>>* _weakReferencesByResource;
        Luasel::MetaTable* _weakReferenceMetaTable;
        std::list<Luasel::Ref /* fake ref UserData */>* _fakeReferences;
        Luasel::MetaTable* _fakeReferenceMetaTable;
        Luasel::Ref* _invalidRef;

    public:
        WeakResourceRefManager(Luasel::Interpreter& interpreter, ManagerType& resourceManager, bool useFakeReferences = false) :
            _interpreter(interpreter),
            _resourceManager(resourceManager),
            _nextReferenceId(1),
            _fakeReferences(0),
            _fakeReferenceMetaTable(0)
        {
            this->_weakReferences = new std::unordered_map<Uint32, std::list<std::pair<WeakResourceRefType*, Luasel::Ref>>>();
            this->_weakReferencesByResource = new std::map<WeakResourceRefType, std::pair<Uint32, std::list<std::pair<WeakResourceRefType*, Luasel::Ref>>*>>();
            this->_weakReferenceMetaTable = &Luasel::MetaTable::Create(interpreter, WeakResourceRefType());
            this->_weakReferenceMetaTable->SetMethod("Lock", std::bind(&WeakResourceRefManager::_WeakReferenceLock, this, std::placeholders::_1));
            this->_weakReferenceMetaTable->SetMetaMethod(Luasel::MetaTable::Serialize, std::bind(&WeakResourceRefManager::_WeakReferenceSerialize, this, std::placeholders::_1));
            if (useFakeReferences)
            {
                this->_fakeReferences = new std::list<Luasel::Ref>();
                this->_fakeReferenceMetaTable = &Luasel::MetaTable::Create(interpreter, FakeReference(this->_interpreter.MakeNil()));
                this->_fakeReferenceMetaTable->SetMetaMethod(Luasel::MetaTable::Index, std::bind(&WeakResourceRefManager::_FakeReferenceIndex, this, std::placeholders::_1));
                this->_fakeReferenceMetaTable->SetMetaMethod(Luasel::MetaTable::NewIndex, std::bind(&WeakResourceRefManager::_FakeReferenceNewIndex, this, std::placeholders::_1));
            }
            this->_invalidRef = new Luasel::Ref(this->_interpreter.Make(WeakResourceRefType()));
        }

        ~WeakResourceRefManager()
        {
            Tools::Delete(this->_invalidRef);
            Tools::Delete(this->_weakReferences);
            Tools::Delete(this->_weakReferencesByResource);
            Tools::Delete(this->_fakeReferences);
        }

        bool UsingFakeReferences() const
        {
            return this->_fakeReferences != nullptr;
        }

        std::pair<Uint32, Luasel::Ref> NewResource(WeakResourceRefType const& resource)
        {
            auto it = this->_weakReferencesByResource->find(resource);
            if (it == this->_weakReferencesByResource->end())
            {
                while (!this->_nextReferenceId // 0 est la valeur spéciale "pas de ref", on la saute
                        || this->_weakReferences->count(this->_nextReferenceId))
                    ++this->_nextReferenceId;
                Uint32 newId = this->_nextReferenceId++;
                Luasel::Ref userData = this->_interpreter.Make(resource);
                auto& refList = this->_weakReferences->operator[](newId);
                refList.push_back(std::make_pair(userData.To<WeakResourceRefType*>(), userData));
                this->_weakReferencesByResource->insert(std::make_pair(resource, std::make_pair(newId, &refList)));
                return std::make_pair(newId, userData);
            }
            else
            {
                assert(!it->second.second->empty());
                return std::make_pair(it->second.first, it->second.second->front().second);
            }
        }

        Luasel::Ref NewUnloadedResource(WeakResourceRefType const& resource)
        {
            WeakResourceRefType unloadedResource = resource;
            unloadedResource.SetLoaded(false);
            return this->_interpreter.Make(unloadedResource);
        }

        void InvalidateResource(Uint32 refId) throw(std::runtime_error)
        {
            auto it = this->_weakReferences->find(refId);
            if (it == this->_weakReferences->end())
                throw std::runtime_error("WeakResourceRefManager::InvalidateResource: No resource with id " + Tools::ToString(refId));
            auto itList = it->second.begin();
            auto itListEnd = it->second.end();
            assert(!it->second.empty());
            this->_weakReferencesByResource->erase(*itList->first);
            for (; itList != itListEnd; ++itList)
                itList->first->Invalidate(this->_resourceManager);
            this->_weakReferences->erase(it);
        }

        Luasel::Ref GetInvalidWeakReference() const
        {
            return *this->_invalidRef;
        }

        Luasel::Ref GetWeakReference(Uint32 refId) const throw(std::runtime_error)
        {
            auto it = this->_weakReferences->find(refId);
            if (it == this->_weakReferences->end())
                throw std::runtime_error("WeakResourceRefManager::GetWeakReference: No weak reference with id " + Tools::ToString(refId));
            assert(!it->second.empty());
            return it->second.front().second;
        }

        // ne pas garder la reference longtemps...
        WeakResourceRefType& GetResource(Uint32 refId) throw(std::runtime_error)
        {
            auto it = this->_weakReferences->find(refId);
            if (it == this->_weakReferences->end())
                throw std::runtime_error("WeakResourceRefManager::GetResource: No resource with id " + Tools::ToString(refId));
            assert(!it->second.empty());
            return *it->second.front().first;
        }

        // retourne true si il y avait des fake references a enlever
        bool InvalidateAllFakeReferences()
        {
            if (!this->_fakeReferences)
                return false;
            bool ret = false;
            auto it = this->_fakeReferences->begin();
            auto itEnd = this->_fakeReferences->end();
            for (; it != itEnd; ++it)
            {
                ret = true;
                Luasel::Ref& ref = *it;
                ref.To<FakeReference*>()->Invalidate();
            }
            this->_fakeReferences->clear();
            return ret;
        }

    private:
        void _WeakReferenceSerialize(Luasel::CallHelper& helper)
        {
            WeakResourceRefType* weakRef = helper.PopArg("WeakResourceRefManager::_WeakReferenceSerialize: Missing argument for __serialize metamethod").To<WeakResourceRefType*>();
            helper.PushRet(this->_interpreter.MakeString(weakRef->Serialize(this->_resourceManager)));
        }

        void _WeakReferenceLock(Luasel::CallHelper& helper)
        {
            Luasel::Ref ref = helper.PopArg("WeakResourceRefManager::_WeakReferenceLock: Missing self argument for __index metamethod");
            WeakResourceRefType* weakRef = ref.To<WeakResourceRefType*>();
            if (!weakRef->IsLoaded())
            {
                weakRef->TryToLoad(this->_resourceManager);
                auto it = this->_weakReferencesByResource->find(*weakRef);
                if (it == this->_weakReferencesByResource->end())
                {
                    helper.PushRet(helper.GetInterpreter().MakeNil());
                    return;
                }
                weakRef->SetLoaded(true);
                it->second.second->push_back(std::make_pair(weakRef, ref));
            }
            if (weakRef->IsValid(this->_resourceManager))
            {
                if (this->_fakeReferences)
                {
                    Luasel::Ref fakeRef = this->_interpreter.Make(FakeReference(weakRef->GetReference(this->_resourceManager)));
                    this->_fakeReferences->push_back(fakeRef);
                    helper.PushRet(fakeRef);
                }
                else
                    helper.PushRet(weakRef->GetReference(this->_resourceManager));
            }
            else
                helper.PushRet(helper.GetInterpreter().MakeNil());
        }

        void _FakeReferenceIndex(Luasel::CallHelper& helper)
        {
            FakeReference* ref = helper.PopArg("WeakResourceRefManager::_FakeReferenceIndex: Missing self argument for __index metamethod").To<FakeReference*>();
            Luasel::Ref key = helper.PopArg("WeakResourceRefManager::_FakeReferenceIndex: Missing key argument for __index metamethod");
            if (!ref->IsValid())
                throw std::runtime_error("WeakResourceRefManager::_FakeReferenceIndex: This reference was invalidated - you must not keep true references to resources, only weak references");
            Luasel::Ref trueRef = ref->GetReference();
            if (trueRef.IsTable())
            {
                Luasel::Ref value = trueRef[key];
                if (value.Exists())
                {
                    helper.PushRet(value); // action normale (table avec champ existant)
                    return;
                }
            }
            else if (!trueRef.IsUserData()) // methode __index sur un objet qui n'est pas une table, ni un userdata
                throw std::runtime_error("WeakResourceRefManager::_FakeReferenceIndex: __index metamethod called on a reference of type " + trueRef.GetTypeName());
            // reproduction du comportement du lua pour __index
            Luasel::Ref metaTable = trueRef.GetMetaTable();
            if (metaTable.IsTable())
            {
                Luasel::Ref index = metaTable["__index"];
                if (index.IsFunction())
                {
                    helper.GetArgList().clear();
                    helper.PushArg(trueRef);
                    helper.PushArg(key);
                    index(helper);
                    return;
                }
                else if (index.IsTable())
                {
                    helper.PushRet(index[key]); // ne permet pas le chainage de __index (c'est un rawget) -- comportement different du lua normal
                    return;
                }
                else if (index.Exists()) // si __index est à nil on Get quand meme
                    throw std::runtime_error("WeakResourceRefManager::_FakeReferenceIndex: __index metamethod is of type " + index.GetTypeName());
            }
            else if (trueRef.IsTable())
                helper.PushRet(this->_interpreter.MakeNil()); // table sans metatable, champ vide
            else
                throw std::runtime_error("WeakResourceRefManager::_FakeReferenceIndex: No metatable set for this UserData for call to __index");
        }

        void _FakeReferenceNewIndex(Luasel::CallHelper& helper)
        {
            FakeReference* ref = helper.PopArg("WeakResourceRefManager::_FakeReferenceNewIndex: Missing self argument for __newindex metamethod").To<FakeReference*>();
            Luasel::Ref key = helper.PopArg("WeakResourceRefManager::_FakeReferenceNewIndex: Missing key argument for __newindex metamethod");
            Luasel::Ref value = helper.PopArg("WeakResourceRefManager::_FakeReferenceNewIndex: Missing value argument for __newindex metamethod");
            if (!ref->IsValid())
                throw std::runtime_error("WeakResourceRefManager::_FakeReferenceNewIndex: This reference was invalidated - you must not keep true references to resources, only weak references");
            Luasel::Ref trueRef = ref->GetReference();
            if (trueRef.IsTable())
            {
                if (trueRef[key].Exists())
                {
                    trueRef.Set(key, value); // action normale (table avec champ existant)
                    return;
                }
            }
            else if (!trueRef.IsUserData()) // method __newindex sur un objet qui n'est pas une table, ni un userdata
                throw std::runtime_error("WeakResourceRefManager::_FakeReferenceNewIndex: __newindex metamethod called on a reference of type " + trueRef.GetTypeName());
            // reproduction du comportement du lua pour __newindex
            Luasel::Ref metaTable = trueRef.GetMetaTable();
            if (metaTable.IsTable())
            {
                Luasel::Ref newIndex = metaTable["__newindex"];
                if (newIndex.IsFunction())
                {
                    newIndex(trueRef, key, value);
                    return;
                }
                else if (newIndex.IsTable())
                {
                    newIndex.Set(key, value); // ne permet pas le chainage de __newindex (c'est un rawset) -- comportement different du lua normal
                    return;
                }
                else if (newIndex.Exists()) // si __newindex est à nil, on Set quand meme
                    throw std::runtime_error("WeakResourceRefManager::_FakeReferenceNewIndex: __newindex metamethod is of type " + newIndex.GetTypeName());
            }
            else if (trueRef.IsTable())
                trueRef.Set(key, value); // table sans metatable, champ vide
            else
                throw std::runtime_error("WeakResourceRefManager::_FakeReferenceNewIndex: No metatable set for this UserData for call to __newindex");
        }
    };

}}

#endif
