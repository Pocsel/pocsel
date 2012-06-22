#ifndef __CLIENT_GAME_SHAPERENDERER_HPP__
#define __CLIENT_GAME_SHAPERENDERER_HPP__

namespace Tools {
    class IRenderer;
    namespace Renderers {
        class IShaderProgram;
        class IShaderParameter;
        class IVertexBuffer;
        class ITexture2D;
    }
}

namespace Common { namespace Physics {
    class Node;
}}

namespace Client { namespace Game {
    class Game;
}}

namespace Client { namespace Game {

    class ShapeRenderer
    {
    private:
        Game& _game;
        Tools::IRenderer& _renderer;
        Tools::Renderers::IShaderProgram* _shader;
        //Tools::Renderers::IShaderParameter* _shaderTexture;
        //Tools::Renderers::IShaderParameter* _shaderTime;
        std::unique_ptr<Tools::Renderers::IVertexBuffer> _vertexBuffer;
        Uint64 _elapsedTime;

    public:
        ShapeRenderer(Game& game);
        ~ShapeRenderer();
        void Render(Common::Physics::Node const& node);

    private:
        void _InitVertexBuffer();
    };

}}

#endif
