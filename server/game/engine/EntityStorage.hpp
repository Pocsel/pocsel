#ifndef __SERVER_GAME_ENGINE_ENTITYSTORAGE_HPP__
#define __SERVER_GAME_ENGINE_ENTITYSTORAGE_HPP__

namespace Tools { namespace Lua {
    class Interpreter;
    class Ref;
    class CallHelper;
}}

namespace Server { namespace Game { namespace Engine {

    class EntityStorage
    {
    private:
        Tools::Lua::Ref* _table;
    public:
        EntityStorage(Tools::Lua::Interpreter& interpreter);
        ~EntityStorage();
        void Length(Tools::Lua::CallHelper& helper);
        void Index(Tools::Lua::CallHelper& helper);
        void NewIndex(Tools::Lua::CallHelper& helper);
        void Serialize(Tools::Lua::CallHelper& helper);
    };

}}}

#endif
