#ifndef __CLIENT_GAME_CUBETYPE_HPP__
#define __CLIENT_GAME_CUBETYPE_HPP__

#include "common/CubeType.hpp"

namespace Tools { namespace Lua {
    class Interpreter;
}}

namespace Client {
    class Client;
    namespace Resources {
        class Effect;
        class ResourceManager;
    }
}

namespace Client { namespace Game {

    class CubeType : public Common::CubeType
    {
    public:
        struct Textures
        {
            union
            {
                struct
                {
                    Uint32 top;
                    Uint32 bottom;
                    Uint32 right;
                    Uint32 left;
                    Uint32 front;
                    Uint32 back;
                };
                Uint32 ids[6];
            };

            Textures() : top(0), bottom(0), right(0), left(0), front(0), back(0) {}
        } textures;

        struct Effects
        {
            union
            {
                struct
                {
                    Resources::Effect* top;
                    Resources::Effect* bottom;
                    Resources::Effect* right;
                    Resources::Effect* left;
                    Resources::Effect* front;
                    Resources::Effect* back;
                };
                Resources::Effect* effects[6];
            };

            Effects() : top(0), bottom(0), right(0), left(0), front(0), back(0) {}
        } effects;

        // TODO !
        //struct
        //{
        //    std::unique_ptr<Resources::IEffectParameter> top;
        //    std::unique_ptr<Resources::IEffectParameter> bottom;
        //    std::unique_ptr<Resources::IEffectParameter> right;
        //    std::unique_ptr<Resources::IEffectParameter> left;
        //    std::unique_ptr<Resources::IEffectParameter> front;
        //    std::unique_ptr<Resources::IEffectParameter> back;
        //} effectParameters;

        explicit CubeType(Common::CubeType const& cubeType);
        void Load(Resources::ResourceManager& resourceMgr, Tools::Lua::Interpreter& interpreter);
    };

}}

#endif
