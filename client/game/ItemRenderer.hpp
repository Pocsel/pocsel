#ifndef __CLIENT_GAME_ITEMRENDERER_HPP__
#define __CLIENT_GAME_ITEMRENDERER_HPP__

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
        class Item;
    }
    namespace Resources {
        class ITexture;
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
        Tools::Renderers::IShaderParameter* _shaderBoneMatrix;
        Uint32 _elapsedTime;

    public:
        ItemRenderer(Game& game);
        ~ItemRenderer();
        void Render(Item const& item);
        void Update(Uint32 time);

    private:
    };

}}

#endif
