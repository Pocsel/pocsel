#ifndef __COMMON_ITEMTYPE_HPP__
#define __COMMON_ITEMTYPE_HPP__

namespace Common {

    struct ItemType
    {
        Uint32 const id;
        Uint32 const plugin_id;
        std::string const name;
        std::string const lua;

        ItemType(Uint32 id, Uint32 plugin_id,
                std::string const& name,
                std::string const& lua) :
            id(id),
            plugin_id(plugin_id),
            name(name),
            lua(lua)
        {}

        ItemType(ItemType const& item) :
            id(item.id),
            plugin_id(item.plugin_id),
            name(item.name),
            lua(item.lua)
        {}
    private:
        ItemType& operator =(ItemType const&);
    };

#endif
