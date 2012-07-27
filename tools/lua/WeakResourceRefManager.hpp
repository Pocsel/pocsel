#ifndef __TOOLS_LUA_WEAKRESOURCEREFMANAGER_HPP__
#define __TOOLS_LUA_WEAKRESOURCEREFMANAGER_HPP__

#include "tools/lua/Interpreter.hpp"

namespace Tools { namespace Lua {

    template<typename WeakResourceRefType, typename ManagerType> class WeakResourceRefManager :
        private boost::noncopyable
    {
    private:
        struct FakeReference
        {
            FakeReference(Ref const& ref) : trueReference(ref) {}
            bool IsValid() const { return this->trueReference.IsValid(); }
            void Invalidate() { this->trueReference.Unref(); }
            Ref GetReference() const { return this->trueReference; }
            Tools::Lua::Ref trueReference;
        };

    private:
        Interpreter& _interpreter;
        ManagerType& _resourceManager;
        Uint32 _nextReferenceId;
        std::unordered_map<Uint32 /* ref id */, Ref /* weak ref UserData */>* _weakReferences;
        MetaTable* _weakReferenceMetaTable;
        std::list<Ref /* fake ref UserData */>* _fakeReferences;
        MetaTable* _fakeReferenceMetaTable;
        Ref* _invalidRef;

    public:
        WeakResourceRefManager(Interpreter& interpreter, ManagerType& resourceManager, WeakResourceRefType const& invalidRef = WeakResourceRefType(), bool useFakeReferences = false) :
            _interpreter(interpreter),
            _resourceManager(resourceManager),
            _nextReferenceId(1),
            _fakeReferences(0),
            _fakeReferenceMetaTable(0)
        {
            this->_weakReferences = new std::unordered_map<Uint32, Ref>();
            this->_weakReferenceMetaTable = &MetaTable::Create(interpreter, WeakResourceRefType());
            this->_weakReferenceMetaTable->SetMethod("Lock", std::bind(&WeakResourceRefManager::_WeakReferenceLock, this, std::placeholders::_1));
            if (useFakeReferences)
            {
                this->_fakeReferences = new std::list<Ref>();
                this->_fakeReferenceMetaTable = &MetaTable::Create(interpreter, FakeReference(this->_interpreter.MakeNil()));
                this->_fakeReferenceMetaTable->SetMetaMethod(MetaTable::Index, std::bind(&WeakResourceRefManager::_FakeReferenceIndex, this, std::placeholders::_1));
                this->_fakeReferenceMetaTable->SetMetaMethod(MetaTable::NewIndex, std::bind(&WeakResourceRefManager::_FakeReferenceNewIndex, this, std::placeholders::_1));
            }
            this->_invalidRef = new Tools::Lua::Ref(this->_interpreter.Make(invalidRef));
        }

        ~WeakResourceRefManager()
        {
            Tools::Delete(this->_invalidRef);
            Tools::Delete(this->_weakReferences);
            Tools::Delete(this->_fakeReferences);
        }

        std::pair<Uint32, Ref> NewResource(WeakResourceRefType const& resource)
        {
            while (!this->_nextReferenceId // 0 est la valeur spéciale "pas de ref", on la saute
                    || this->_weakReferences->count(this->_nextReferenceId))
                ++this->_nextReferenceId;
            Uint32 newId = this->_nextReferenceId++;
            Ref userData = this->_interpreter.Make(resource);
            return *this->_weakReferences->insert(std::make_pair(newId, userData)).first;
        }

        void InvalidateResource(Uint32 refId) throw(std::runtime_error)
        {
            auto it = this->_weakReferences->find(refId);
            if (it == this->_weakReferences->end())
                throw std::runtime_error("WeakResourceRefManager::InvalidateResource: No resource with id " + Tools::ToString(refId));
            Ref& ref = it->second;
            ref.To<WeakResourceRefType*>()->Invalidate(this->_resourceManager);
            this->_weakReferences->erase(it);
        }

        Ref GetInvalidWeakReference() const
        {
            return *this->_invalidRef;
        }

        Ref GetWeakReference(Uint32 refId) const throw(std::runtime_error)
        {
            auto it = this->_weakReferences->find(refId);
            if (it == this->_weakReferences->end())
                throw std::runtime_error("WeakResourceRefManager::GetWeakReference: No weak reference with id " + Tools::ToString(refId));
            return it->second;
        }

        // ne pas garder la reference longtemps...
        WeakResourceRefType& GetResource(Uint32 refId) throw(std::runtime_error)
        {
            auto it = this->_weakReferences->find(refId);
            if (it == this->_weakReferences->end())
                throw std::runtime_error("WeakResourceRefManager::GetResource: No resource with id " + Tools::ToString(refId));
            Ref& ref = it->second;
            return *ref.To<WeakResourceRefType*>();
        }

        void InvalidateAllFakeReferences()
        {
            if (!this->_fakeReferences)
                return;
            auto it = this->_fakeReferences->begin();
            auto itEnd = this->_fakeReferences->end();
            for (; it != itEnd; ++it)
            {
                Ref& ref = *it;
                ref.To<FakeReference*>()->Invalidate();
            }
            this->_fakeReferences->clear();
        }

    private:
        void _WeakReferenceLock(CallHelper& helper)
        {
            WeakResourceRefType* weakRef = helper.PopArg("WeakResourceRefManager::_WeakReferenceCall: Missing self argument for __index metamethod").To<WeakResourceRefType*>();
            if (weakRef->IsValid(this->_resourceManager))
            {
                if (this->_fakeReferences)
                {
                    Ref fakeRef = this->_interpreter.Make(FakeReference(weakRef->GetReference(this->_resourceManager)));
                    this->_fakeReferences->push_back(fakeRef);
                    helper.PushRet(fakeRef);
                }
                else
                    helper.PushRet(weakRef->GetReference(this->_resourceManager));
            }
            else
                helper.PushRet(helper.GetInterpreter().MakeNil());
        }

        void _FakeReferenceIndex(CallHelper& helper)
        {
            FakeReference* ref = helper.PopArg("WeakResourceRefManager::_FakeReferenceIndex: Missing self argument for __index metamethod").To<FakeReference*>();
            Ref key = helper.PopArg("WeakResourceRefManager::_FakeReferenceIndex: Missing key argument for __index metamethod");
            if (!ref->IsValid())
                throw std::runtime_error("WeakResourceRefManager::_FakeReferenceIndex: This reference was invalidated - you must not keep true references to resources, only weak references");
            Ref table = ref->GetReference();
            if (!table.IsTable())
                throw std::runtime_error("WeakResourceRefManager::_FakeReferenceIndex: __index metamethod called on a reference of type " + table.GetTypeName());
            Ref value = table[key];
            // reproduction du comportement du lua pour __index
            if (!value.Exists())
            {
                Ref metaTable = table.GetMetaTable();
                if (metaTable.IsTable())
                {
                    Ref index = metaTable["__index"];
                    if (index.IsFunction())
                    {
                        helper.GetArgList().clear();
                        helper.PushArg(table);
                        helper.PushArg(key);
                        index(helper);
                        return;
                    }
                    else if (index.IsTable())
                    {
                        helper.PushRet(index[key]); // ne permet pas le chainage de __index (c'est un rawset) -- comportement different du lua normal
                        return;
                    }
                    else if (index.Exists()) // si __index est à nil on Get quand meme
                        throw std::runtime_error("WeakResourceRefManager::_FakeReferenceIndex: __index metamethod is of type " + index.GetTypeName());
                }
            }
            // action normal sans __index
            helper.PushRet(value);
        }

        void _FakeReferenceNewIndex(CallHelper& helper)
        {
            FakeReference* ref = helper.PopArg("WeakResourceRefManager::_FakeReferenceNewIndex: Missing self argument for __newindex metamethod").To<FakeReference*>();
            Ref key = helper.PopArg("WeakResourceRefManager::_FakeReferenceNewIndex: Missing key argument for __newindex metamethod");
            Ref value = helper.PopArg("WeakResourceRefManager::_FakeReferenceNewIndex: Missing value argument for __newindex metamethod");
            if (!ref->IsValid())
                throw std::runtime_error("WeakResourceRefManager::_FakeReferenceNewIndex: This reference was invalidated - you must not keep true references to resources, only weak references");
            Ref table = ref->GetReference();
            if (!table.IsTable())
                throw std::runtime_error("WeakResourceRefManager::_FakeReferenceNewIndex: __newindex metamethod called on a reference of type " + table.GetTypeName());
            // reproduction du comportement du lua pour __newindex
            if (!table[key].Exists())
            {
                Ref metaTable = table.GetMetaTable();
                if (metaTable.IsTable())
                {
                    Ref newIndex = metaTable["__newindex"];
                    if (newIndex.IsFunction())
                    {
                        newIndex(table, key, value);
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
            }
            // action normale sans __newindex
            ref->GetReference().Set(key, value);
        }
    };

}}

#endif
