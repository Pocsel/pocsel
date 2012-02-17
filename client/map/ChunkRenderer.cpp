#include "client2/precompiled.hpp"

#include "client2/Client.hpp"
#include "client2/game/Game.hpp"
#include "client2/game/Player.hpp"
#include "client2/map/Chunk.hpp"
#include "client2/map/ChunkRenderer.hpp"
#include "client2/map/Map.hpp"
#include "client2/resources/LocalResourceManager.hpp"
#include "client2/window/Window.hpp"

#include "tools/Frustum.hpp"

namespace Client { namespace Map {

    ChunkRenderer::ChunkRenderer(Game::Game& game)
        : _game(game),
        _renderer(game.GetClient().GetWindow().GetRenderer()),
        _baseVbo(0)
    {
        this->_InitBaseVbo();
        this->_shader = &this->_game.GetClient().GetLocalResourceManager().GetShader("Chunk.cgfx");
        this->_shaderTexture = this->_shader->GetParameter("cubeTexture").release();
        auto const& cubeTypes = this->_game.GetCubeTypeManager().GetCubeTypes();
        this->_cubeInfo.resize(cubeTypes.size());
        for (size_t i = 0; i < cubeTypes.size(); ++i)
        {
            this->_cubeInfo[i].id = cubeTypes[i].id;
            this->_cubeInfo[i].texture = &this->_game.GetResourceManager().GetTexture2D(cubeTypes[i].textures.top);
            // TODO: les bonnes valeurs
            this->_cubeInfo[i].top = Tools::Vector3f(0, 0, 1);
            this->_cubeInfo[i].bottom = Tools::Vector3f(0, 0, 1);
            this->_cubeInfo[i].front = Tools::Vector3f(0, 0, 1);
            this->_cubeInfo[i].back = Tools::Vector3f(0, 0, 1);
            this->_cubeInfo[i].left = Tools::Vector3f(0, 0, 1);
            this->_cubeInfo[i].right = Tools::Vector3f(0, 0, 1);
            this->_cubeInfo[i].isTransparent = this->_cubeInfo[i].texture->HasAlpha();
        }
    }

    ChunkRenderer::~ChunkRenderer()
    {
        Tools::Delete(this->_baseVbo);
        Tools::Delete(this->_shaderTexture);
    }

    void ChunkRenderer::RefreshDisplay(Chunk& chunk)
    {
        ChunkMesh* mesh = chunk.GetMesh();
        if (mesh == 0)
        {
            mesh = new ChunkMesh(chunk);
            chunk.SetMesh(std::unique_ptr<ChunkMesh>(mesh));
        }
        mesh->Refresh(this->_game, *this);
    }

    void ChunkRenderer::Render()
    {
        auto const& camera = this->_game.GetPlayer().GetCamera();
        auto pos = camera.position.GetVector<double>();
        auto viewProj = 
            Tools::Matrix4<double>::CreateLookAt(pos, Tools::Vector3d(pos + Tools::Vector3d(camera.direction)), Tools::Vector3d(0, 1, 0))
            * Tools::Matrix4<double>(camera.projection);
        Tools::Frustum frustum(viewProj);

        std::deque<Chunk*> chunkToRender;
        this->_game.GetMap().GetChunkManager().ForeachIn(frustum,
            [&chunkToRender](Chunk& chunk)
            {
                chunkToRender.push_back(&chunk);
            });

        do
        {
            this->_shader->BeginPass();
            this->_baseVbo->Bind();
            for (size_t i = 0; i < this->_cubeInfo.size(); ++i)
            {
                this->_cubeInfo[i].texture->Bind();
                this->_shaderTexture->Set(*this->_cubeInfo[i].texture);

                for (auto it = chunkToRender.begin(), ite = chunkToRender.end(); it != ite; ++it)
                {
                    auto mesh = (*it)->GetMesh();
                    if (!mesh || !mesh->HasCubeType(this->_cubeInfo[i].id))
                        continue;

                    this->_renderer.SetModelMatrix(Tools::Matrix4<float>::CreateTranslation(Common::Position((*it)->coords, Tools::Vector3f(0)) - camera.position));
                    mesh->Render(this->_cubeInfo[i].id, this->_renderer);
                }

                this->_cubeInfo[i].texture->Unbind();
            }
            this->_baseVbo->Unbind();
        } while (this->_shader->EndPass());
    }

    void ChunkRenderer::_InitBaseVbo()
    {
        unsigned int const frontTopLeft = 0;
        unsigned int const frontTopRight = 1;
        unsigned int const backTopRight = 2;
        unsigned int const backTopLeft = 3;
        unsigned int const backBottomLeft = 4;
        unsigned int const backBottomRight = 5;
        unsigned int const frontBottomRight = 6;
        unsigned int const frontBottomLeft = 7;

        unsigned int const normalFront = 0;
        unsigned int const normalTop = 1;
        unsigned int const normalRight = 2;
        unsigned int const normalBottom = 3;
        unsigned int const normalLeft = 4;
        unsigned int const normalBack = 5;

        float const cubeVertices[][3] = {
            {0, 1, 1},
            {1, 1, 1},
            {1, 1, 0},
            {0, 1, 0},
            {0, 0, 0},
            {1, 0, 0},
            {1, 0, 1},
            {0, 0, 1},
        };
        unsigned int const faceIndices[] = {
            frontTopLeft, frontBottomLeft, frontBottomRight, frontTopRight, // front
            frontTopLeft, frontTopRight, backTopRight, backTopLeft, // top
            backTopRight, frontTopRight, frontBottomRight, backBottomRight, // right
            frontBottomLeft, backBottomLeft, backBottomRight, frontBottomRight, // bottom
            frontTopLeft, backTopLeft, backBottomLeft, frontBottomLeft, // left
            backTopLeft, backTopRight, backBottomRight, backBottomLeft, // back
        };

        float const cubeNormals[][3] = {
            {0, 0, 1},
            {0, 1, 0},
            {1, 0, 0},
            {0, -1, 0},
            {-1, 0, 0},
            {0, 0, -1}
        };
        unsigned int const faceIndicesN[] = {
            normalFront, normalFront, normalFront, normalFront, // front
            normalTop, normalTop, normalTop, normalTop, // top
            normalRight, normalRight, normalRight, normalRight, // right
            normalBottom, normalBottom, normalBottom, normalBottom, // bottom
            normalLeft, normalLeft, normalLeft, normalLeft, // left
            normalBack, normalBack, normalBack, normalBack, // back
        };

        float const cubeTexCoords[][2] = {
            {0, 0},
            {1, 0},
            {1, 1},
            {0, 1},
        };
        unsigned int const faceIndicesT[] = {
            0, 1, 2, 3, // front
            0, 1, 2, 3, // top
            0, 1, 2, 3, // right
            0, 1, 2, 3, // bottom
            0, 1, 2, 3, // left
            0, 1, 2, 3, // back
        };

        float* vertices = new float[Common::ChunkSize3 * sizeof(faceIndices) / sizeof(faceIndices[0]) * (3+3+2)];// * sizeof(float)];

        unsigned int x, y, z, i;
        size_t offset = 0;
        float const* vert;
        for (x = 0; x < Common::ChunkSize; ++x)
        {
            for (y = 0; y < Common::ChunkSize; ++y)
            {
                for (z = 0; z < Common::ChunkSize; ++z)
                {
                    for (i = 0; i < sizeof(faceIndices) / sizeof(faceIndices[0]); ++i)
                    {
                        vert = cubeVertices[faceIndices[i]];
                        vertices[offset++] = vert[0] + x;
                        vertices[offset++] = vert[1] + y;
                        vertices[offset++] = vert[2] + z;
                        std::memcpy(&vertices[offset], &cubeNormals[faceIndicesN[i]], sizeof(cubeNormals[0]));
                        offset += 3;
                        vertices[offset++] = cubeTexCoords[faceIndicesT[i]][0];
                        vertices[offset++] = cubeTexCoords[faceIndicesT[i]][1];
                    }
                }
            }
        }

        this->_baseVbo = this->_renderer.CreateVertexBuffer().release();
        this->_baseVbo->PushVertexAttribute(Tools::Renderers::DataType::Float, Tools::Renderers::VertexAttributeUsage::Position, 3); // position
        this->_baseVbo->PushVertexAttribute(Tools::Renderers::DataType::Float, Tools::Renderers::VertexAttributeUsage::Normal, 3); // Normales
        this->_baseVbo->PushVertexAttribute(Tools::Renderers::DataType::Float, Tools::Renderers::VertexAttributeUsage::TexCoord, 2); // Textures
        this->_baseVbo->SetData(Common::ChunkSize3 * sizeof(faceIndices) / sizeof(faceIndices[0]) * (3+3+2) *sizeof(float), vertices, Tools::Renderers::VertexBufferUsage::Static);
    }
}}
