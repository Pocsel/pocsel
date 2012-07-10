#ifndef __CLIENT_GAME_ENGINE_BODYMANAGER_HPP__
#define __CLIENT_GAME_ENGINE_BODYMANAGER_HPP__

#include "client/game/engine/BodyType.hpp"

namespace Client { namespace Game { namespace Engine {
    class Engine;
}}}

namespace Client { namespace Game { namespace Engine {

    class BodyManager :
        private boost::noncopyable
    {
    private:
        Engine& _engine;
        std::vector<std::unique_ptr<BodyType>> _bodyTypes;
        Uint32 _nbBodyTypes;

    public:
        BodyManager(Engine& engine, Uint32 nbBodyTypes);
        void AddBodyType(std::unique_ptr<BodyType> bodyType);
    private:
        void _AskOneType();
    };

}}}

#endif
