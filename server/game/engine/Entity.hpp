#ifndef __SERVER_GAME_ENGINE_ENTITY_HPP__
#define __SERVER_GAME_ENGINE_ENTITY_HPP__

#include "tools/lua/Ref.hpp"

namespace Server { namespace Game { namespace Engine {

    class EntityType;

    class Entity :
        private boost::noncopyable
    {
    protected:
        EntityType* _type;
        Tools::Lua::Ref _self;

    public:
        Entity(Tools::Lua::Interpreter& interpreter, Uint32 id, EntityType* type);
        virtual ~Entity();
        EntityType const& GetType() const { return *this->_type; }
        Tools::Lua::Ref const& GetSelf() const { return this->_self; }
        Tools::Lua::Ref GetStorage() const;
        void SetStorage(Tools::Lua::Ref const& storage);
        void Disable(Tools::Lua::Interpreter& interpreter);
        void Enable(Tools::Lua::Interpreter& interpreter);
    };

}}}

#endif
