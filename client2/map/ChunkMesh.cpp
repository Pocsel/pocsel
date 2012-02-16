#include "client2/precompiled.hpp"

#include "client2/Client.hpp"
#include "client2/window/Window.hpp"
#include "client2/game/Game.hpp"
#include "client2/map/Chunk.hpp"
#include "client2/map/ChunkManager.hpp"
#include "client2/map/ChunkMesh.hpp"
#include "client2/map/ChunkRenderer.hpp"
#include "client2/map/Map.hpp"

namespace Client { namespace Map {

    ChunkMesh::ChunkMesh(Chunk& chunk)
        : _chunk(chunk),
        _textureCoords(0),
        _triangleCount(0),
        _hasTransparentCube(false)
    {
    }

    ChunkMesh::~ChunkMesh()
    {
        for (auto it = this->_indices.begin(), ite = this->_indices.end(); it != ite; ++it)
            Tools::Delete(it->second.first);
        Tools::Delete(this->_textureCoords);
    }

    void ChunkMesh::Refresh(Game::Game& game, ChunkRenderer& chunkRenderer)
    {
        auto& cm = game.GetMap().GetChunkManager();
        auto chunkLeft  = cm.GetChunk(this->_chunk.coords + Common::BaseChunk::CoordsType(-1,  0,  0)),
            chunkRight  = cm.GetChunk(this->_chunk.coords + Common::BaseChunk::CoordsType( 1,  0,  0)),
            chunkFront  = cm.GetChunk(this->_chunk.coords + Common::BaseChunk::CoordsType( 0,  0,  1)),
            chunkBack   = cm.GetChunk(this->_chunk.coords + Common::BaseChunk::CoordsType( 0,  0, -1)),
            chunkTop    = cm.GetChunk(this->_chunk.coords + Common::BaseChunk::CoordsType( 0,  1,  0)),
            chunkBottom = cm.GetChunk(this->_chunk.coords + Common::BaseChunk::CoordsType( 0, -1,  0));

        for (auto it = this->_indices.begin(), ite = this->_indices.end(); it != ite; ++it)
            Tools::Delete(it->second.first);
        this->_indices.clear();
        delete this->_textureCoords;
        this->_textureCoords = 0;
        this->_triangleCount = 0;
        if (this->_chunk.IsEmpty())
            return;

        Common::BaseChunk::CubeType const* cubes = this->_chunk.GetCubes();

        unsigned long x, y, z, offset, cubeOffset;
        Common::BaseChunk::CubeType type;
        Common::BaseChunk::CubeType nearType;

        std::map<Common::BaseChunk::CubeType, std::vector<unsigned int>> indices;

        static THREAD_LOCAL Tools::Vector2f* textureCoords = 0;
        if (textureCoords == 0)
            textureCoords = new Tools::Vector2f[Common::ChunkSize3 * 4 * 6]; // 4 sommets, 6 faces

        for (x = 0; x < Common::ChunkSize; ++x)
        {
            for (y = 0; y < Common::ChunkSize; ++y)
            {
                for (z = 0; z < Common::ChunkSize; ++z)
                {
                    cubeOffset = x + y * Common::ChunkSize + z * Common::ChunkSize2;
                    if (cubes[cubeOffset] != 0)
                    {
                        offset = (x * Common::ChunkSize2 + y * Common::ChunkSize + z) * 24;
                        type = cubes[cubeOffset];
                        auto const& cubeType = chunkRenderer.GetCubeInfo(type);
                        if (cubeType.isTransparent)
                            this->_hasTransparentCube = true;
#define FRONT_FACE 0
#define TOP_FACE 4
#define RIGHT_FACE 8
#define BOTTOM_FACE 12
#define LEFT_FACE 16
#define BACK_FACE 20

#define GET_INFO(cubeType)  (chunkRenderer.GetCubeInfo(cubeType))
#define APPEND_INDICES(getType, decal, FACE) \
                        do { \
                            nearType = (getType); \
                            if (nearType == 0 || (nearType != type && GET_INFO(nearType).isTransparent)) \
                            { \
                                indices[type].push_back(offset + decal + 0); \
                                indices[type].push_back(offset + decal + 1); \
                                indices[type].push_back(offset + decal + 2); \
                                indices[type].push_back(offset + decal + 0); \
                                indices[type].push_back(offset + decal + 2); \
                                indices[type].push_back(offset + decal + 3); \
                                textureCoords[offset + decal + 0] = Tools::Vector2f(cubeType.FACE.u, cubeType.FACE.v); \
                                textureCoords[offset + decal + 1] = Tools::Vector2f(cubeType.FACE.v + cubeType.FACE.w, cubeType.FACE.v); \
                                textureCoords[offset + decal + 2] = Tools::Vector2f(cubeType.FACE.v + cubeType.FACE.w, cubeType.FACE.v + cubeType.FACE.w); \
                                textureCoords[offset + decal + 3] = Tools::Vector2f(cubeType.FACE.v, cubeType.FACE.v + cubeType.FACE.w); \
                            } \
                        } while (0)

                        if (x != Common::ChunkSize - 1)
                            APPEND_INDICES(cubes[cubeOffset + 1], RIGHT_FACE, right);
                        else if (x == Common::ChunkSize - 1 && chunkRight != 0)
                            APPEND_INDICES(chunkRight->GetCube(0, y, z), RIGHT_FACE, right);

                        if (y != Common::ChunkSize - 1)
                            APPEND_INDICES(cubes[cubeOffset + Common::ChunkSize], TOP_FACE, top);
                        else if (y == Common::ChunkSize - 1 && chunkTop != 0)
                            APPEND_INDICES(chunkTop->GetCube(x, 0, z), TOP_FACE, top);

                        if (z != Common::ChunkSize - 1)
                            APPEND_INDICES(cubes[cubeOffset + Common::ChunkSize2], FRONT_FACE, front);
                        else if (z == Common::ChunkSize - 1 && chunkFront != 0)
                            APPEND_INDICES(chunkFront->GetCube(x, y, 0), FRONT_FACE, front);

                        if (x != 0)
                            APPEND_INDICES(cubes[cubeOffset - 1], LEFT_FACE, left);
                        else if (x == 0 && chunkLeft != 0)
                            APPEND_INDICES(chunkLeft->GetCube(Common::ChunkSize - 1, y, z), LEFT_FACE, left);

                        if (y != 0)
                            APPEND_INDICES(cubes[cubeOffset - Common::ChunkSize], BOTTOM_FACE, bottom);
                        else if (y == 0 && chunkBottom != 0)
                            APPEND_INDICES(chunkBottom->GetCube(x, Common::ChunkSize - 1, z), BOTTOM_FACE, bottom);

                        if (z != 0)
                            APPEND_INDICES(cubes[cubeOffset - Common::ChunkSize2], BACK_FACE, back);
                        else if (z == 0 && chunkBack != 0)
                            APPEND_INDICES(chunkBack->GetCube(x, y, Common::ChunkSize - 1), BACK_FACE, back);

#undef APPEND_INDICES
#undef GET_INFO

                    }
                }
            }
        }

        if (indices.size() == 0)
            return;

        this->_textureCoords = game.GetClient().GetWindow().GetRenderer().CreateVertexBuffer().release();
        this->_textureCoords->PushVertexAttribute(Tools::Renderers::DataType::Float, Tools::Renderers::VertexAttributeUsage::TexCoord, 2);
        this->_textureCoords->SetData((Common::ChunkSize3 * 4 * 6) * sizeof(Tools::Vector2f), textureCoords, Tools::Renderers::VertexBufferUsage::Static);
        for (auto it = indices.begin(), ite = indices.end(); it != ite; ++it)
        {
            if (it->second.size() == 0)
                continue;
            
            auto ib = game.GetClient().GetWindow().GetRenderer().CreateIndexBuffer().release();
            ib->SetData(sizeof(unsigned int) * it->second.size(), it->second.data());
            this->_indices[it->first] = std::pair<Tools::Renderers::IIndexBuffer*, Uint32>(ib, (Uint32)it->second.size());
            this->_triangleCount += (Uint32)(indices[it->first].size() / 3);
        }
    }

    void ChunkMesh::Render(int cubeType, Tools::IRenderer& renderer)
    {
        if (this->_chunk.IsEmpty() || this->_indices[cubeType].first == 0)
            return;
        this->_textureCoords->Bind();
        this->_indices[cubeType].first->Bind();
        renderer.UpdateCurrentParameters();
        renderer.DrawElements(this->_indices[cubeType].second, Tools::Renderers::DataType::UnsignedInt, 0);
        this->_indices[cubeType].first->Unbind();
        this->_textureCoords->Unbind();
    }

}}
