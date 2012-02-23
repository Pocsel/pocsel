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
        nbIndices(m.nbIndices)
    {
        m.indices = 0;
        m.nbIndices = 0;
    }

    ChunkMesh::Mesh::~Mesh()
    {
        Tools::Delete(this->indices);
    }

    ChunkMesh::Mesh& ChunkMesh::Mesh::operator =(ChunkMesh::Mesh&& m)
    {
        std::swap(this->indices, m.indices);
        std::swap(this->nbIndices, m.nbIndices);
        return *this;
    }

    ChunkMesh::ChunkMesh(Chunk& chunk)
        : _chunk(chunk),
        _vertices(0),
        _triangleCount(0),
        _hasTransparentCube(false),
        _isComputed(false),
        _tmpNbVertices(0),
        _tmpVertices(0)
    {
    }

    ChunkMesh::~ChunkMesh()
    {
        Tools::Delete(this->_vertices);
        Tools::DeleteTab(this->_tmpVertices);
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
                //Uint32 packed = ((normal.x + 1) << 4) | ((normal.y + 1) << 2) | (normal.z + 1);
                //this->normal = ((float)packed) / ((float) (1 << 6));
                //packed = (texture.x << 1) | texture.y;
                //this->texture = ((float)packed) / ((float) (1 << 2));
            }
        };

        inline void VerticesPushFace(std::vector<Vertex>& vertices, unsigned int& offset, Tools::Vector3f const& p, int idx)
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
                Tools::Vector2f(1, 0),
                Tools::Vector2f(0, 0),
                Tools::Vector2f(0, 1),
                Tools::Vector2f(1, 1),
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
            vertices[offset++] = Vertex(positions[*(pos++)] + p, normals[idx], textures[0]);
            vertices[offset++] = Vertex(positions[*(pos++)] + p, normals[idx], textures[3]);
            vertices[offset++] = Vertex(positions[*(pos++)] + p, normals[idx], textures[2]);
            vertices[offset++] = Vertex(positions[*(pos++)] + p, normals[idx], textures[1]);
        }

        inline void IndicesPushFace(std::vector<unsigned int>& indices, unsigned int voffset)
        {
            indices.push_back(voffset - 4);
            indices.push_back(voffset - 3);
            indices.push_back(voffset - 2);
            indices.push_back(voffset - 2);
            indices.push_back(voffset - 1);
            indices.push_back(voffset - 4);
        }
    }

    bool ChunkMesh::Refresh(Game::Game& game, ChunkRenderer& chunkRenderer)
    {
        boost::lock_guard<boost::mutex> lock(this->_refreshMutex);
        auto& cm = game.GetMap().GetChunkManager();
        auto chunkLeft  = cm.GetChunk(this->_chunk.coords + Common::BaseChunk::CoordsType(-1,  0,  0)),
            chunkRight  = cm.GetChunk(this->_chunk.coords + Common::BaseChunk::CoordsType( 1,  0,  0)),
            chunkFront  = cm.GetChunk(this->_chunk.coords + Common::BaseChunk::CoordsType( 0,  0,  1)),
            chunkBack   = cm.GetChunk(this->_chunk.coords + Common::BaseChunk::CoordsType( 0,  0, -1)),
            chunkTop    = cm.GetChunk(this->_chunk.coords + Common::BaseChunk::CoordsType( 0,  1,  0)),
            chunkBottom = cm.GetChunk(this->_chunk.coords + Common::BaseChunk::CoordsType( 0, -1,  0));

        this->_hasTransparentCube = false;
        if (chunkLeft   == 0 ||
            chunkRight  == 0 ||
            chunkFront  == 0 ||
            chunkBack   == 0 ||
            chunkTop    == 0 ||
            chunkBottom == 0)
        {
            this->_tmpNbVertices = 0;
            delete [] this->_tmpVertices;
            this->_tmpVertices = 0;
            this->_tmpIndices.clear();
            return false;
        }

        if (this->_chunk.IsEmpty())
            return true;

        auto const& cubeTypes = game.GetCubeTypeManager().GetCubeTypes();
        Common::BaseChunk::CubeType const* cubes = this->_chunk.GetCubes();
        Common::BaseChunk::CubeType nearType;

        std::vector<Vertex> vertices((Common::ChunkSize3 * 6 * 4) / 2);
        std::map<Uint32, std::vector<unsigned int>> indices;
        unsigned int voffset = 0;

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

                    auto const& cubeType = cubeTypes[cubes[cubeOffset] - 1];
                    Tools::Vector3f p(x, y, z);

                    // Right
                    nearType = 0;
                    if (x != Common::ChunkSize - 1)
                        nearType = cubes[cubeOffset + 1];
                    else
                        nearType = chunkRight->GetCube(0, y, z);
                    if (nearType == 0 || (nearType != cubeType.id && chunkRenderer.GetTexture(cubeTypes[nearType - 1].textures.right).HasAlpha()))
                    {
                        this->_hasTransparentCube = this->_hasTransparentCube || chunkRenderer.GetTexture(cubeType.textures.right).HasAlpha();
                        VerticesPushFace(vertices, voffset, p, 2);
                        IndicesPushFace(indices[cubeType.textures.right], voffset);
                    }

                    // Top
                    nearType = 0;
                    if (y != Common::ChunkSize - 1)
                        nearType = cubes[cubeOffset + Common::ChunkSize];
                    else
                        nearType = chunkTop->GetCube(x, 0, z);
                    if (nearType == 0 || (nearType != cubeType.id && chunkRenderer.GetTexture(cubeTypes[nearType - 1].textures.top).HasAlpha()))
                    {
                        this->_hasTransparentCube = this->_hasTransparentCube || chunkRenderer.GetTexture(cubeType.textures.top).HasAlpha();
                        VerticesPushFace(vertices, voffset, p, 1);
                        IndicesPushFace(indices[cubeType.textures.top], voffset);
                    }

                    // Front
                    nearType = 0;
                    if (z != Common::ChunkSize - 1)
                        nearType = cubes[cubeOffset + Common::ChunkSize2];
                    else if (z == Common::ChunkSize - 1)
                        nearType = chunkFront->GetCube(x, y, 0);
                    if (nearType == 0 || (nearType != cubeType.id && chunkRenderer.GetTexture(cubeTypes[nearType - 1].textures.front).HasAlpha()))
                    {
                        this->_hasTransparentCube = this->_hasTransparentCube || chunkRenderer.GetTexture(cubeType.textures.front).HasAlpha();
                        VerticesPushFace(vertices, voffset, p, 0);
                        IndicesPushFace(indices[cubeType.textures.front], voffset);
                    }

                    // Left
                    nearType = 0;
                    if (x != 0)
                        nearType = cubes[cubeOffset - 1];
                    else
                        nearType = chunkLeft->GetCube(Common::ChunkSize - 1, y, z);
                    if (nearType == 0 || (nearType != cubeType.id && chunkRenderer.GetTexture(cubeTypes[nearType - 1].textures.left).HasAlpha()))
                    {
                        this->_hasTransparentCube = this->_hasTransparentCube || chunkRenderer.GetTexture(cubeType.textures.left).HasAlpha();
                        VerticesPushFace(vertices, voffset, p, 4);
                        IndicesPushFace(indices[cubeType.textures.left], voffset);
                    }

                    // Bottom
                    nearType = 0;
                    if (y != 0)
                        nearType = cubes[cubeOffset - Common::ChunkSize];
                    else
                        nearType = chunkBottom->GetCube(x, Common::ChunkSize - 1, z);
                    if (nearType == 0 || (nearType != cubeType.id && chunkRenderer.GetTexture(cubeTypes[nearType - 1].textures.bottom).HasAlpha()))
                    {
                        this->_hasTransparentCube = this->_hasTransparentCube || chunkRenderer.GetTexture(cubeType.textures.bottom).HasAlpha();
                        VerticesPushFace(vertices, voffset, p, 3);
                        IndicesPushFace(indices[cubeType.textures.bottom], voffset);
                    }

                    // Back
                    nearType = 0;
                    if (z != 0)
                        nearType = cubes[cubeOffset - Common::ChunkSize2];
                    else
                        nearType = chunkBack->GetCube(x, y, Common::ChunkSize - 1);
                    if (nearType == 0 || (nearType != cubeType.id && chunkRenderer.GetTexture(cubeTypes[nearType - 1].textures.back).HasAlpha()))
                    {
                        this->_hasTransparentCube = this->_hasTransparentCube || chunkRenderer.GetTexture(cubeType.textures.back).HasAlpha();
                        VerticesPushFace(vertices, voffset, p, 5);
                        IndicesPushFace(indices[cubeType.textures.back], voffset);
                    }
                }
            }
        }

        if (voffset == 0)
            return true;

        this->_tmpIndices = std::move(indices);
        this->_tmpNbVertices = voffset;
        this->_tmpVertices = new float[voffset * (3+3+2)];
        std::memcpy(this->_tmpVertices, vertices.data(), voffset * (3+3+2) * sizeof(float));
        return true;
    }

    bool ChunkMesh::RefreshGraphics(Tools::IRenderer& renderer)
    {
        if (!this->_refreshMutex.try_lock())
            return false;
        boost::lock_guard<boost::mutex> lock(this->_refreshMutex, boost::adopt_lock);
        // on clear tout
        this->_meshes.clear();
        delete this->_vertices;
        this->_vertices = 0;
        this->_triangleCount = 0;

        if (this->_chunk.IsEmpty() || this->_tmpNbVertices == 0)
            return true;

        this->_vertices = renderer.CreateVertexBuffer().release();
        this->_vertices->PushVertexAttribute(Tools::Renderers::DataType::Float, Tools::Renderers::VertexAttributeUsage::Position, 3); // position
        this->_vertices->PushVertexAttribute(Tools::Renderers::DataType::Float, Tools::Renderers::VertexAttributeUsage::Normal, 3); // Normales + Textures
        this->_vertices->PushVertexAttribute(Tools::Renderers::DataType::Float, Tools::Renderers::VertexAttributeUsage::TexCoord, 2); // Textures
        this->_vertices->SetData(this->_tmpNbVertices * (3+3+2) * sizeof(*this->_tmpVertices), this->_tmpVertices, Tools::Renderers::VertexBufferUsage::Static);
        for (auto it = this->_tmpIndices.begin(), ite = this->_tmpIndices.end(); it !=ite; ++it)
        {
            if (it->second.size() == 0)
                continue;
            Mesh m;
            m.indices = renderer.CreateIndexBuffer().release();
            m.indices->SetData(sizeof(unsigned int) * it->second.size(), it->second.data());
            m.nbIndices = (Uint32)it->second.size();
            this->_triangleCount += m.nbIndices / 3;
            this->_meshes[it->first] = std::move(m);
        }

        delete [] this->_tmpVertices;
        this->_tmpVertices = 0;
        this->_tmpIndices.clear();
        return true;
    }

    void ChunkMesh::Render(Uint32 textureId, Tools::IRenderer& renderer)
    {
        if (this->_triangleCount == 0)
            return;
        Mesh& m = this->_meshes[textureId];
        if (m.nbIndices == 0)
            return;
        this->_vertices->Bind();
        m.indices->Bind();
        renderer.DrawElements(m.nbIndices, Tools::Renderers::DataType::UnsignedInt, 0);
        m.indices->Unbind();
        this->_vertices->Unbind();
    }

}}
