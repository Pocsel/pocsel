#ifndef __CLIENT_GAME_ITEMRENDERER_HPP__
#define __CLIENT_GAME_ITEMRENDERER_HPP__

#include "client/resources/ITexture.hpp"
#include "client/resources/Md5Model.hpp"

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
        class Game;
    }
}

namespace Client { namespace Game {

    class ItemRenderer
    {
    private:
        Game& _game;
        Tools::IRenderer& _renderer;
        Tools::Renderers::IShaderProgram* _shader;
        Tools::Renderers::IShaderParameter* _shaderTexture;
        Tools::Renderers::IShaderParameter* _shaderTime;
        Resources::Md5Model* _md5Model;
        //std::unique_ptr<Tools::Renderers::IVertexBuffer> _vertexBuffer;
        Tools::Renderers::ITexture2D* _texture;
        Uint32 _elapsedTime;

    public:
        ItemRenderer(Game& game);
        ~ItemRenderer();
        void Render(Common::OrientedPosition const& pos);
        void Update(Uint32 time);

    private:
        void _InitVertexBuffer();
    };

}}

#endif

