#ifndef __CLIENT_GAME_MODELRENDERER_HPP__
#define __CLIENT_GAME_MODELRENDERER_HPP__

#include "common/Position.hpp"

namespace Common {
    struct OrientedPosition;
}
namespace Tools {
    class IRenderer;
    namespace Renderers {
        class IShaderParameter;
        class IShaderProgram;
        class IVertexBuffer;
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
        Tools::Renderers::IShaderProgram* _shader;
        Tools::Renderers::IShaderParameter* _shaderTexture;
        Tools::Renderers::IShaderParameter* _shaderBoneMatrix;

    public:
        ModelRenderer(Game& game);
        ~ModelRenderer();
        void Render(Engine::Model const& model, Common::Position const& pos);

    private:
    };

}}

#endif
