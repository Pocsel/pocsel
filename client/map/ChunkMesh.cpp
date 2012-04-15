#include "client/precompiled.hpp"

#include "client/Client.hpp"
#include "client/window/Window.hpp"
#include "client/game/CubeType.hpp"
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
            glm::fvec3 position;
            glm::fvec3 normal;
            glm::fvec2 texture;
            Vertex() {}
            Vertex(glm::fvec3 const& position, glm::fvec3 const& normal, glm::fvec2 const& texture)
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

        inline void VerticesPushFace(std::vector<Vertex>& vertices, unsigned int& offset, glm::fvec3 const& p, int idx)
        {
            static glm::fvec3 const positions[] = {
                glm::fvec3(0, 1, 1), // frontTopLeft = 0;
                glm::fvec3(1, 1, 1), // frontTopRight = 1;
                glm::fvec3(1, 1, 0), // backTopRight = 2;
                glm::fvec3(0, 1, 0), // backTopLeft = 3;
                glm::fvec3(0, 0, 0), // backBottomLeft = 4;
                glm::fvec3(1, 0, 0), // backBottomRight = 5;
                glm::fvec3(1, 0, 1), // frontBottomRight = 6;
                glm::fvec3(0, 0, 1), // frontBottomLeft = 7;
            };
            static glm::fvec3 const normals[] = {
                glm::fvec3(0, 0, 1), // front = 0;
                glm::fvec3(0, 1, 0), // top = 1;
                glm::fvec3(1, 0, 0), // right = 2;
                glm::fvec3(0, -1, 0), // bottom = 3;
                glm::fvec3(-1, 0, 0), // left = 4;
                glm::fvec3(0, 0, -1), // back = 5;
            };
            glm::fvec2 const textures[] = {
                glm::fvec2(1, 0),
                glm::fvec2(0, 0),
                glm::fvec2(0, 1),
                glm::fvec2(1, 1),
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

        inline Chunk::CubeType GetCube(Chunk::CubeType const* cubes, unsigned int x, unsigned int y, unsigned int z)
        {
            return cubes[x + y * Common::ChunkSize + z * Common::ChunkSize2];
        }
    }

    bool ChunkMesh::Refresh(ChunkRenderer& chunkRenderer, std::vector<Game::CubeType> cubeTypes, std::shared_ptr<Chunk::CubeType> myCubes, std::vector<std::shared_ptr<Chunk::CubeType>> neighbors)
    {
        boost::lock_guard<boost::mutex> lock(this->_refreshMutex);
        auto const& chunkLeft   = neighbors[0].get();
        auto const& chunkRight  = neighbors[1].get();
        auto const& chunkFront  = neighbors[2].get();
        auto const& chunkBack   = neighbors[3].get();
        auto const& chunkTop    = neighbors[4].get();
        auto const& chunkBottom = neighbors[5].get();

//        if (chunkLeft   == 0 ||
//            chunkRight  == 0 ||
//            chunkFront  == 0 ||
//            chunkBack   == 0 ||
//            chunkTop    == 0 ||
//            chunkBottom == 0)
//        {
//            this->_tmpNbVertices = 0;
//            Tools::Delete([] this->_tmpVertices);
//            this->_tmpVertices = 0;
//            this->_tmpIndices.clear();
//            return false;
//        }

        this->_tmpNbVertices = 0;
        Tools::DeleteTab(this->_tmpVertices);
        this->_tmpVertices = 0;
        this->_tmpIndices.clear();

        if (this->_chunk.IsEmpty())
        {
            this->_hasTransparentCube = false;
            return true;
        }

        bool hasTransparentCube = false;

        Common::BaseChunk::CubeType const* cubes = myCubes.get();
        Common::BaseChunk::CubeType nearType;

        // On récupère les textures pour y accéder plus rapidement
        auto const& textureMap = chunkRenderer.GetAllTextures();
        std::vector<Resources::ITexture*> textures(textureMap.rbegin()->first + 1);
        for (auto it = textureMap.begin(), ite = textureMap.end(); it != ite; ++it)
            textures[it->first] = it->second.get();

        std::vector<Vertex> vertices((Common::ChunkSize3 * 6 * 4) / 2 * 2);
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
                    glm::fvec3 p((float)x, (float)y, (float)z);

                    // Right
                    nearType = 0;
                    if (x != Common::ChunkSize - 1)
                        nearType = cubes[cubeOffset + 1];
                    else if (chunkRight != 0)
                        nearType = GetCube(chunkRight, 0, y, z);
                    if (nearType == 0 || (nearType != cubeType.id && textures[cubeTypes[nearType - 1].textures.right]->HasAlpha()))
                    {
                        hasTransparentCube = hasTransparentCube || textures[cubeType.textures.right]->HasAlpha();
                        VerticesPushFace(vertices, voffset, p, 2);
                        IndicesPushFace(indices[cubeType.textures.right], voffset);
                    }

                    // Top
                    nearType = 0;
                    if (y != Common::ChunkSize - 1)
                        nearType = cubes[cubeOffset + Common::ChunkSize];
                    else if (chunkTop != 0)
                        nearType = GetCube(chunkTop, x, 0, z);
                    if (nearType == 0 || (nearType != cubeType.id && textures[cubeTypes[nearType - 1].textures.top]->HasAlpha()))
                    {
                        hasTransparentCube = hasTransparentCube || textures[cubeType.textures.top]->HasAlpha();
                        VerticesPushFace(vertices, voffset, p, 1);
                        IndicesPushFace(indices[cubeType.textures.top], voffset);
                    }

                    // Front
                    nearType = 0;
                    if (z != Common::ChunkSize - 1)
                        nearType = cubes[cubeOffset + Common::ChunkSize2];
                    else if (chunkFront != 0)
                        nearType = GetCube(chunkFront, x, y, 0);
                    if (nearType == 0 || (nearType != cubeType.id && textures[cubeTypes[nearType - 1].textures.front]->HasAlpha()))
                    {
                        hasTransparentCube = hasTransparentCube || textures[cubeType.textures.front]->HasAlpha();
                        VerticesPushFace(vertices, voffset, p, 0);
                        IndicesPushFace(indices[cubeType.textures.front], voffset);
                    }

                    // Left
                    nearType = 0;
                    if (x != 0)
                        nearType = cubes[cubeOffset - 1];
                    else if (chunkLeft != 0)
                        nearType = GetCube(chunkLeft, Common::ChunkSize - 1, y, z);
                    if (nearType == 0 || (nearType != cubeType.id && textures[cubeTypes[nearType - 1].textures.left]->HasAlpha()))
                    {
                        hasTransparentCube = hasTransparentCube || textures[cubeType.textures.left]->HasAlpha();
                        VerticesPushFace(vertices, voffset, p, 4);
                        IndicesPushFace(indices[cubeType.textures.left], voffset);
                    }

                    // Bottom
                    nearType = 0;
                    if (y != 0)
                        nearType = cubes[cubeOffset - Common::ChunkSize];
                    else if (chunkBottom != 0)
                        nearType = GetCube(chunkBottom, x, Common::ChunkSize - 1, z);
                    if (nearType == 0 || (nearType != cubeType.id && textures[cubeTypes[nearType - 1].textures.bottom]->HasAlpha()))
                    {
                        hasTransparentCube = hasTransparentCube || textures[cubeType.textures.bottom]->HasAlpha();
                        VerticesPushFace(vertices, voffset, p, 3);
                        IndicesPushFace(indices[cubeType.textures.bottom], voffset);
                    }

                    // Back
                    nearType = 0;
                    if (z != 0)
                        nearType = cubes[cubeOffset - Common::ChunkSize2];
                    else if (chunkBack != 0)
                        nearType = GetCube(chunkBack, x, y, Common::ChunkSize - 1);
                    if (nearType == 0 || (nearType != cubeType.id && textures[cubeTypes[nearType - 1].textures.back]->HasAlpha()))
                    {
                        hasTransparentCube = hasTransparentCube || textures[cubeType.textures.back]->HasAlpha();
                        VerticesPushFace(vertices, voffset, p, 5);
                        IndicesPushFace(indices[cubeType.textures.back], voffset);
                    }
                }
            }
        }

        this->_hasTransparentCube = hasTransparentCube;
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
        Tools::Delete(this->_vertices);
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
            m.indices->SetData(Tools::Renderers::DataType::UnsignedInt, sizeof(unsigned int) * it->second.size(), it->second.data());
            m.nbIndices = (Uint32)it->second.size();
            this->_triangleCount += m.nbIndices / 3;
            this->_meshes[it->first] = std::move(m);
        }

        Tools::DeleteTab(this->_tmpVertices);
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
        renderer.DrawElements(m.nbIndices);
        m.indices->Unbind();
        this->_vertices->Unbind();
    }

}}
