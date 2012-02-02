#ifndef __CLIENT_GUI_PRIMITIVES_CHUNK_HPP__
#define __CLIENT_GUI_PRIMITIVES_CHUNK_HPP__

#include <memory>
#include <vector>

namespace Common {
    struct Camera;
}

namespace Tools {
    class IRenderer;
    class Timer;
    namespace Renderers {
        class IIndexBuffer;
        class IShaderProgram;
        class IShaderParameter;
        class ITexture2D;
        class IVertexBuffer;
    }
}

namespace Client {

    class Chunk;
    class World;

    namespace Gui { namespace Primitives {

        class Chunk
        {
        public:
            struct CubeInfo
            {
                unsigned int textureIndex;
                float height; // hauteur d'une case
                float top; // debut de la texture du top
                float bottom;
                float side;
                bool isTransparent;
            };
            struct MeshIndices
            {
                CubeInfo const* info;
                Tools::Renderers::IIndexBuffer* indices;
                Uint32 indicesCount;

                MeshIndices() : info(0), indices(0), indicesCount(0) {}
                MeshIndices(CubeInfo const& info, Tools::Renderers::IIndexBuffer* indices, Uint32 indicesCount);
                MeshIndices(MeshIndices&& obj);
                MeshIndices& operator =(MeshIndices&& obj);
                ~MeshIndices();
            private:
                MeshIndices(MeshIndices const&);
            };

        private:
            static Tools::Renderers::IVertexBuffer* _vertexBuffer;
            static std::vector<Tools::Renderers::IVertexBuffer*>* _texBuffers;
            static Tools::Renderers::IShaderProgram* _shader;
            static Tools::Renderers::IShaderParameter* _textureParameter;
            static std::vector<Tools::Renderers::ITexture2D*>* _textures;
            static std::vector<CubeInfo>* _cubeInfos;

            static Tools::Timer* _timer;
            static Tools::Renderers::IShaderParameter* _timerParameter;

            Client::Chunk& _infos;
            std::vector<MeshIndices> _indices;
            Uint32 _triangleCount;
            bool _hasTransparentCube;

        public:
            Chunk(Client::Chunk& infos);
            ~Chunk();

            bool HasTransparentCube() const { return this->_hasTransparentCube; }
            void Refresh(Tools::IRenderer& renderer, World& world);
            void Render(Common::Camera const& camera, Tools::IRenderer& renderer);
            static void Init(Tools::IRenderer& renderer,
                             std::unique_ptr<std::vector<Tools::Renderers::ITexture2D*>>& textures,
                             std::unique_ptr<std::vector<CubeInfo>>& cubeInfos);
            static void Bind();
            static void UnBind();

        private:
        };

    }}
}

#endif
