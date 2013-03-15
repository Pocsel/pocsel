#ifndef __CLIENT_GAME_SHAPERENDERER_HPP__
#define __CLIENT_GAME_SHAPERENDERER_HPP__

namespace Tools {
    namespace Gfx {
        class IRenderer;
        class IShaderParameter;
        class IVertexBuffer;
        class ITexture2D;
        namespace Effect {
            class Effect;
        }
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
        Tools::Gfx::IRenderer& _renderer;
        Tools::Gfx::Effect::Effect* _shader;

        Tools::Gfx::Utils::Sphere* _sphere;
        Tools::Gfx::Utils::Cube* _cube;

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
