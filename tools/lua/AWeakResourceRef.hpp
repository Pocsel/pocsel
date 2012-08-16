#ifndef __TOOLS_LUA_AWEAKRESOURCEREF_HPP__
#define __TOOLS_LUA_AWEAKRESOURCEREF_HPP__

namespace Tools { namespace Lua {

    /*
     * Struct simple contenant le minimum de données permettant de retrouver une resource.
     * Doit être copiable.
     * La classe fille doit implémenter l'opérateur <.
     */
    template<typename ManagerType> struct AWeakResourceRef
    {
        AWeakResourceRef() : _isLoaded(true) {}

        virtual ~AWeakResourceRef() {}
        virtual bool IsValid(ManagerType const&) const = 0;
        virtual void Invalidate(ManagerType const&) = 0;
        virtual Ref GetReference(ManagerType const&) const = 0;
        virtual std::string Serialize(ManagerType const&) const { return "return nil"; }

        bool IsLoaded() const { return this->_isLoaded; }
        void SetLoaded(bool isLoaded = true) { this->_isLoaded = isLoaded; }

    private:
        bool _isLoaded;
    };

}}

#endif
