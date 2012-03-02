#ifndef __CLIENT_GAME_TARGETEDCUBERENDERER_HPP__
#define __CLIENT_GAME_TARGETEDCUBERENDERER_HPP__

#include "client/resources/ITexture.hpp"

namespace Common {
    struct CubePosition;
}

namespace Tools {
    class IRenderer;
    namespace Renderers {
        class IShaderParameter;
        class IShaderProgram;
        class IVertexBuffer;
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
        Tools::Renderers::IVertexBuffer* _vertexBuffer;
        Tools::Renderers::ITexture2D* _texture;
        Uint32 _elapsedTime;

    public:
        TargetedCubeRenderer(Game& game);
        ~TargetedCubeRenderer();
        void Render(Common::CubePosition const& pos);
        void Update(Uint32 time);

    private:
        void _InitVertexBuffer();
    };

}}

#endif
