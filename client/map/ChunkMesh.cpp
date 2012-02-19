#include "client/precompiled.hpp"

#include "client/Client.hpp"
#include "client/window/Window.hpp"
#include "client/game/Game.hpp"
#include "client/map/Chunk.hpp"
#include "client/map/ChunkManager.hpp"
#include "client/map/ChunkMesh.hpp"
#include "client/map/ChunkRenderer.hpp"
#include "client/map/Map.hpp"

namespace Client { namespace Map {

    ChunkMesh::Mesh::Mesh(Mesh&& m)
        : indices(m.indices),
        vertices(m.vertices),
        nbIndices(m.nbIndices)
    {
        m.indices = 0;
        m.vertices = 0;
        m.nbIndices = 0;
    }

    ChunkMesh::Mesh::~Mesh()
    {
        Tools::Delete(this->indices);
        Tools::Delete(this->vertices);
    }

    ChunkMesh::Mesh& ChunkMesh::Mesh::operator =(ChunkMesh::Mesh&& m)
    {
        std::swap(this->indices, m.indices);
        std::swap(this->vertices, m.vertices);
        std::swap(this->nbIndices, m.nbIndices);
        return *this;
    }

    ChunkMesh::ChunkMesh(Chunk& chunk)
        : _chunk(chunk),
        _triangleCount(0),
        _hasTransparentCube(false),
        _isComputed(false)
    {
    }

    ChunkMesh::~ChunkMesh()
    {
    }

    namespace {
        struct Vertex
        {
            Tools::Vector3f position;
            Tools::Vector3f normal;
            Tools::Vector2f texture;
            Vertex() {}
            Vertex(Tools::Vector3f const& position, Tools::Vector3f const& normal, Tools::Vector2f const& texture)
                : position(position),
                normal(normal),
                texture(texture)
            {
            }
        };

        inline void VerticesPushFace(std::vector<Vertex>& vertices, unsigned int& offset, Tools::Vector3f const& p, int idx, Tools::Vector3f const& texCoords)
        {
            static Tools::Vector3f const positions[] = {
                Tools::Vector3f(0, 1, 1), // frontTopLeft = 0;
                Tools::Vector3f(1, 1, 1), // frontTopRight = 1;
                Tools::Vector3f(1, 1, 0), // backTopRight = 2;
                Tools::Vector3f(0, 1, 0), // backTopLeft = 3;
                Tools::Vector3f(0, 0, 0), // backBottomLeft = 4;
                Tools::Vector3f(1, 0, 0), // backBottomRight = 5;
                Tools::Vector3f(1, 0, 1), // frontBottomRight = 6;
                Tools::Vector3f(0, 0, 1), // frontBottomLeft = 7;
            };
            static Tools::Vector3f const normals[] = {
                Tools::Vector3f(0, 0, 1), // front = 0;
                Tools::Vector3f(0, 1, 0), // top = 1;
                Tools::Vector3f(1, 0, 0), // right = 2;
                Tools::Vector3f(0, -1, 0), // bottom = 3;
                Tools::Vector3f(-1, 0, 0), // left = 4;
                Tools::Vector3f(0, 0, -1), // back = 5;
            };
            Tools::Vector2f const textures[] = {
                Tools::Vector2f(texCoords.w, texCoords.w),
                Tools::Vector2f(texCoords.w, 0),
                Tools::Vector2f(0, 0),
                Tools::Vector2f(0, texCoords.w),
            };
            static int positionIndices[][4] = {
                {6, 1, 0, 7}, // front = 0;
                {0, 1, 2, 3}, // top = 1;
                {5, 2, 1, 6}, // right = 2;
                {4, 5, 6, 7}, // bottom = 3;
                {7, 0, 3, 4}, // left = 4;
                {4, 3, 2, 5}, // back = 5;
            };

            int *pos = positionIndices[idx];
            vertices[offset++] = Vertex(positions[*(pos++)] + p, normals[idx], textures[0] + texCoords.GetXY());
            vertices[offset++] = Vertex(positions[*(pos++)] + p, normals[idx], textures[3] + texCoords.GetXY());
            vertices[offset++] = Vertex(positions[*(pos++)] + p, normals[idx], textures[2] + texCoords.GetXY());
            vertices[offset++] = Vertex(positions[*(pos++)] + p, normals[idx], textures[1] + texCoords.GetXY());
        }

        inline void IndicesPushFace(std::vector<unsigned int>& indices, unsigned int& offset, unsigned int voffset)
        {
            indices[offset++] = voffset - 4;
            indices[offset++] = voffset - 3;
            indices[offset++] = voffset - 2;
            indices[offset++] = voffset - 2;
            indices[offset++] = voffset - 1;
            indices[offset++] = voffset - 4;
        }
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

        this->_meshes.clear();
        this->_triangleCount = 0;
        this->_hasTransparentCube = false;
        if (chunkLeft   == 0 ||
            chunkRight  == 0 ||
            chunkFront  == 0 ||
            chunkBack   == 0 ||
            chunkTop    == 0 ||
            chunkBottom == 0)
            return;

        if (this->_chunk.IsEmpty())
            return;

        Common::BaseChunk::CubeType const* cubes = this->_chunk.GetCubes();
        Common::BaseChunk::CubeType nearType;

        static std::vector<Vertex> vertices((Common::ChunkSize3 * 6 * 4) / 2);
        static std::vector<unsigned int> indices((Common::ChunkSize3 * 6 * 3 * 2) / 2);
        unsigned int ioffset = 0, voffset = 0;

        unsigned int x, y, z, cubeOffset;
        for (z = 0; z < Common::ChunkSize; ++z)
        {
            for (y = 0; y < Common::ChunkSize; ++y)
            {
                for (x = 0; x < Common::ChunkSize; ++x)
                {
                    cubeOffset = x + y * Common::ChunkSize + z * Common::ChunkSize2;
                    if (cubes[cubeOffset] == 0)
                        continue;

                    auto const& cubeType = chunkRenderer.GetCubeInfo(cubes[cubeOffset]);
                    Tools::Vector3f p(x, y, z);

                    // Right
                    nearType = 0;
                    if (x != Common::ChunkSize - 1)
                        nearType = cubes[cubeOffset + 1];
                    else
                        nearType = chunkRight->GetCube(0, y, z);
                    if (nearType != cubeType.id && (nearType == 0 || chunkRenderer.GetCubeInfo(nearType).isTransparent))
                    {
                        this->_hasTransparentCube = this->_hasTransparentCube || cubeType.isTransparent;
                        VerticesPushFace(vertices, voffset, p, 2, cubeType.right);
                        IndicesPushFace(indices, ioffset, voffset);
                    }

                    // Top
                    nearType = 0;
                    if (y != Common::ChunkSize - 1)
                        nearType = cubes[cubeOffset + Common::ChunkSize];
                    else
                        nearType = chunkTop->GetCube(x, 0, z);
                    if (nearType != cubeType.id && (nearType == 0 || chunkRenderer.GetCubeInfo(nearType).isTransparent))
                    {
                        this->_hasTransparentCube = this->_hasTransparentCube || cubeType.isTransparent;
                        VerticesPushFace(vertices, voffset, p, 1, cubeType.top);
                        IndicesPushFace(indices, ioffset, voffset);
                    }

                    // Front
                    nearType = 0;
                    if (z != Common::ChunkSize - 1)
                        nearType = cubes[cubeOffset + Common::ChunkSize2];
                    else if (z == Common::ChunkSize - 1)
                        nearType = chunkFront->GetCube(x, y, 0);
                    if (nearType != cubeType.id && (nearType == 0 || chunkRenderer.GetCubeInfo(nearType).isTransparent))
                    {
                        this->_hasTransparentCube = this->_hasTransparentCube || cubeType.isTransparent;
                        VerticesPushFace(vertices, voffset, p, 0, cubeType.front);
                        IndicesPushFace(indices, ioffset, voffset);
                    }

                    // Left
                    nearType = 0;
                    if (x != 0)
                        nearType = cubes[cubeOffset - 1];
                    else
                        nearType = chunkLeft->GetCube(Common::ChunkSize - 1, y, z);
                    if (nearType != cubeType.id && (nearType == 0 || chunkRenderer.GetCubeInfo(nearType).isTransparent))
                    {
                        this->_hasTransparentCube = this->_hasTransparentCube || cubeType.isTransparent;
                        VerticesPushFace(vertices, voffset, p, 4, cubeType.left);
                        IndicesPushFace(indices, ioffset, voffset);
                    }

                    // Bottom
                    nearType = 0;
                    if (y != 0)
                        nearType = cubes[cubeOffset - Common::ChunkSize];
                    else
                        nearType = chunkBottom->GetCube(x, Common::ChunkSize - 1, z);
                    if (nearType != cubeType.id && (nearType == 0 || chunkRenderer.GetCubeInfo(nearType).isTransparent))
                    {
                        this->_hasTransparentCube = this->_hasTransparentCube || cubeType.isTransparent;
                        VerticesPushFace(vertices, voffset, p, 3, cubeType.bottom);
                        IndicesPushFace(indices, ioffset, voffset);
                    }

                    // Back
                    nearType = 0;
                    if (z != 0)
                        nearType = cubes[cubeOffset - Common::ChunkSize2];
                    else
                        nearType = chunkBack->GetCube(x, y, Common::ChunkSize - 1);
                    if (nearType != cubeType.id && (nearType == 0 || chunkRenderer.GetCubeInfo(nearType).isTransparent))
                    {
                        this->_hasTransparentCube = this->_hasTransparentCube || cubeType.isTransparent;
                        VerticesPushFace(vertices, voffset, p, 5, cubeType.back);
                        IndicesPushFace(indices, ioffset, voffset);
                    }
                }
            }
        }

        if (voffset == 0)
            return;

        Mesh m;
        m.vertices = game.GetClient().GetWindow().GetRenderer().CreateVertexBuffer().release();
        m.vertices->PushVertexAttribute(Tools::Renderers::DataType::Float, Tools::Renderers::VertexAttributeUsage::Position, 3); // position
        m.vertices->PushVertexAttribute(Tools::Renderers::DataType::Float, Tools::Renderers::VertexAttributeUsage::Normal, 3); // Normales
        m.vertices->PushVertexAttribute(Tools::Renderers::DataType::Float, Tools::Renderers::VertexAttributeUsage::TexCoord, 2); // Textures
        m.vertices->SetData(voffset * (3+3+2) * sizeof(float), vertices.data(), Tools::Renderers::VertexBufferUsage::Static);
        Tools::debug << this->_chunk.id << " vertices: " << voffset << " (" << (voffset * (3+3+2) * sizeof(float) / 1024) << "ko)\n";
        m.indices = game.GetClient().GetWindow().GetRenderer().CreateIndexBuffer().release();
        m.indices->SetData(sizeof(unsigned int) * ioffset, indices.data());
        m.nbIndices = (Uint32)ioffset;
        this->_triangleCount += m.nbIndices / 3;
        this->_meshes[0] = std::move(m);
    }

    void ChunkMesh::Render(int cubeType, Tools::IRenderer& renderer)
    {
        if (this->_triangleCount == 0)
            return;
        Mesh& m = this->_meshes[cubeType];
        if (m.nbIndices == 0)
            return;
        m.vertices->Bind();
        m.indices->Bind();
        renderer.DrawElements(m.nbIndices, Tools::Renderers::DataType::UnsignedInt, 0);
        m.indices->Unbind();
        m.vertices->Unbind();
    }

}}
