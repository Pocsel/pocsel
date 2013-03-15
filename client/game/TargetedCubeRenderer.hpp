#ifndef __CLIENT_GAME_TARGETEDCUBERENDERER_HPP__
#define __CLIENT_GAME_TARGETEDCUBERENDERER_HPP__

#include "common/CubePosition.hpp"

namespace Tools { namespace Gfx {
        class IRenderer;
        class IShaderParameter;
        class IVertexBuffer;
        class ITexture2D;
        namespace Effect {
            class Effect;
        }
}}

namespace Client { namespace Game {
    class Game;
}}

namespace Client { namespace Game {

    class TargetedCubeRenderer
    {
    private:
        Game& _game;
        Tools::Gfx::IRenderer& _renderer;
        Tools::Gfx::Effect::Effect* _shader;
        Tools::Gfx::IShaderParameter* _shaderTexture;
        Tools::Gfx::IShaderParameter* _shaderTime;
        std::unique_ptr<Tools::Gfx::IVertexBuffer> _vertexBuffer;
        Tools::Gfx::ITexture2D* _texture;
        Uint64 _elapsedTime;

    public:
        TargetedCubeRenderer(Game& game);
        ~TargetedCubeRenderer();
        void Render(Common::CubePosition const& pos);
        void Update(Uint64 deltaTime);

    private:
        void _InitVertexBuffer();
    };

}}

#endif
