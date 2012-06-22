#ifndef __CLIENT_GAME_SHAPERENDERER_HPP__
#define __CLIENT_GAME_SHAPERENDERER_HPP__

namespace Tools {
    class IRenderer;
    namespace Renderers {
        class IShaderProgram;
        class IShaderParameter;
        class IVertexBuffer;
        class ITexture2D;
        namespace Utils {
            class Cube;
            class Sphere;
        }
    }
}

namespace Common { namespace Physics {
    struct Node;
}}

namespace Client { namespace Game {
    class Game;
}}

class btRigidBody;

namespace Client { namespace Game {

    class ShapeRenderer
    {
    private:
        Game& _game;
        Tools::IRenderer& _renderer;
        Tools::Renderers::IShaderProgram* _shader;
        //Tools::Renderers::IShaderParameter* _shaderTexture;
        //Tools::Renderers::IShaderParameter* _shaderTime;
        //std::unique_ptr<Tools::Renderers::IVertexBuffer> _vertexBuffer;

        Tools::Renderers::Utils::Sphere* _sphere;
        Tools::Renderers::Utils::Cube* _cube;

    public:
        ShapeRenderer(Game& game);
        ~ShapeRenderer();
        void Render(
                btRigidBody const& body); 
        //Common::Physics::Node const& node);
    };

}}

#endif
