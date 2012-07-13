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

namespace Client { namespace Game { namespace Engine {
    class Body;
}}}

class btRigidBody;
class btBoxShape;
class btSphereShape;

namespace Client { namespace Game {

    class Game;

    class ShapeRenderer
    {
    private:
        Game& _game;
        Tools::IRenderer& _renderer;
        Tools::Renderers::IShaderProgram* _shader;

        Tools::Renderers::Utils::Sphere* _sphere;
        Tools::Renderers::Utils::Cube* _cube;

    public:
        ShapeRenderer(Game& game);
        ~ShapeRenderer();
        void Render(Engine::Body const& body);

    private:
        void _RenderBox(
                btBoxShape const* box,
                glm::quat const& orientation,
                glm::vec3 const& pos);
        void _RenderSphere(
                btSphereShape const* sphere,
                glm::quat const& orientation,
                glm::vec3 const& pos);
    };

}}

#endif
