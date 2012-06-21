#ifndef __CLIENT_GAME_MODELRENDERER_HPP__
#define __CLIENT_GAME_MODELRENDERER_HPP__

#include "common/Position.hpp"

namespace Common {
    struct OrientedPosition;
    namespace Physics {
        struct Node;
    }
}
namespace Tools {
    class IRenderer;
    namespace Renderers {
        class IShaderParameter;
        class IShaderProgram;
        class IVertexBuffer;
        namespace Utils {
            class DeferredShading;
        }
    }
}
namespace Client {
    namespace Game {
        namespace Engine {
            class Model;
        }
        class Game;
    }
    namespace Resources {
        class ITexture;
    }
}

namespace Client { namespace Game {

    class ModelRenderer
    {
    private:
        Game& _game;
        Tools::IRenderer& _renderer;

    public:
        ModelRenderer(Game& game);
        void Render(
                Tools::Renderers::Utils::DeferredShading& deferredShading,
                Engine::Model const& model,
                Common::Physics::Node const& pos,
                float updateFlag);

    private:
    };

}}

#endif
