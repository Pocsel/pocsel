#ifndef __CLIENT_GAME_TARGETEDCUBERENDERER_HPP__
#define __CLIENT_GAME_TARGETEDCUBERENDERER_HPP__

#include "common/CubePosition.hpp"

namespace Tools {
    class IRenderer;
    namespace Renderers {
        class IShaderParameter;
        class IShaderProgram;
        class IVertexBuffer;
        class ITexture2D;
    }
}

namespace Client { namespace Game {
    class Game;
}}

namespace Client { namespace Game {

    class TargetedCubeRenderer
    {
    private:
        Game& _game;
        Tools::IRenderer& _renderer;
        Tools::Renderers::IShaderProgram* _shader;
        Tools::Renderers::IShaderParameter* _shaderTexture;
        Tools::Renderers::IShaderParameter* _shaderTime;
        std::unique_ptr<Tools::Renderers::IVertexBuffer> _vertexBuffer;
        Tools::Renderers::ITexture2D* _texture;
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
