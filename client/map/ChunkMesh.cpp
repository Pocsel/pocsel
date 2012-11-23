#include "client/precompiled.hpp"

#include "tools/renderers/utils/material/LuaMaterial.hpp"
#include "tools/window/Window.hpp"

#include "client/Client.hpp"
#include "client/game/CubeType.hpp"
#include "client/game/Game.hpp"
#include "client/map/Chunk.hpp"
#include "client/map/ChunkManager.hpp"
#include "client/map/ChunkMesh.hpp"
#include "client/map/ChunkRenderer.hpp"
#include "client/map/Map.hpp"

using Tools::Renderers::Utils::Material::LuaMaterial;
using Tools::Renderers::Utils::Material::Material;

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
            glm::u16vec3 position;
            Uint16 packed;
            Vertex() {}
            Vertex(glm::fvec3 const& position, glm::fvec3 const& normal, glm::fvec2 const& texture)
                : position(position)
            {
                this->packed = Uint8((normal.x + 1) * 16 + (normal.y + 1) * 4 + (normal.z + 1));
                this->packed += Uint8((texture.x * 2 + texture.y) * (1 << 6));
                //this->packed /= 256.0f;
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

    bool ChunkMesh::Refresh(ChunkRenderer& chunkRenderer,
            std::vector<Game::CubeType> const& cubeTypes,
            std::shared_ptr<Chunk::CubeArray> myCubes,
            std::vector<std::shared_ptr<Chunk::CubeArray>> neighbors)
    {
        boost::lock_guard<boost::mutex> lock(this->_refreshMutex);
        auto const& chunkLeft   = neighbors[0]->data();
        auto const& chunkRight  = neighbors[1]->data();
        auto const& chunkFront  = neighbors[2]->data();
        auto const& chunkBack   = neighbors[3]->data();
        auto const& chunkTop    = neighbors[4]->data();
        auto const& chunkBottom = neighbors[5]->data();

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

        Common::BaseChunk::CubeType const* cubes = myCubes->data();
        Common::BaseChunk::CubeType nearType;

        std::vector<Vertex> vertices((Common::ChunkSize3 * 6 * 4) / 2 * 2);
        std::map<Material*, std::vector<unsigned int>> indices;
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
                    if (nearType == 0 || (nearType != cubeType.id && cubeTypes[nearType - 1].right->GetMaterial().HasAlpha()))
                    {
                        hasTransparentCube = hasTransparentCube || cubeType.right->GetMaterial().HasAlpha();
                        VerticesPushFace(vertices, voffset, p, 2);
                        IndicesPushFace(indices[&cubeType.right->GetMaterial()], voffset);
                    }

                    // Top
                    nearType = 0;
                    if (y != Common::ChunkSize - 1)
                        nearType = cubes[cubeOffset + Common::ChunkSize];
                    else if (chunkTop != 0)
                        nearType = GetCube(chunkTop, x, 0, z);
                    if (nearType == 0 || (nearType != cubeType.id && cubeTypes[nearType - 1].top->GetMaterial().HasAlpha()))
                    {
                        hasTransparentCube = hasTransparentCube || cubeType.top->GetMaterial().HasAlpha();
                        VerticesPushFace(vertices, voffset, p, 1);
                        IndicesPushFace(indices[&cubeType.top->GetMaterial()], voffset);
                    }

                    // Front
                    nearType = 0;
                    if (z != Common::ChunkSize - 1)
                        nearType = cubes[cubeOffset + Common::ChunkSize2];
                    else if (chunkFront != 0)
                        nearType = GetCube(chunkFront, x, y, 0);
                    if (nearType == 0 || (nearType != cubeType.id && cubeTypes[nearType - 1].front->GetMaterial().HasAlpha()))
                    {
                        hasTransparentCube = hasTransparentCube || cubeType.front->GetMaterial().HasAlpha();
                        VerticesPushFace(vertices, voffset, p, 0);
                        IndicesPushFace(indices[&cubeType.front->GetMaterial()], voffset);
                    }

                    // Left
                    nearType = 0;
                    if (x != 0)
                        nearType = cubes[cubeOffset - 1];
                    else if (chunkLeft != 0)
                        nearType = GetCube(chunkLeft, Common::ChunkSize - 1, y, z);
                    if (nearType == 0 || (nearType != cubeType.id && cubeTypes[nearType - 1].left->GetMaterial().HasAlpha()))
                    {
                        hasTransparentCube = hasTransparentCube || cubeType.left->GetMaterial().HasAlpha();
                        VerticesPushFace(vertices, voffset, p, 4);
                        IndicesPushFace(indices[&cubeType.left->GetMaterial()], voffset);
                    }

                    // Bottom
                    nearType = 0;
                    if (y != 0)
                        nearType = cubes[cubeOffset - Common::ChunkSize];
                    else if (chunkBottom != 0)
                        nearType = GetCube(chunkBottom, x, Common::ChunkSize - 1, z);
                    if (nearType == 0 || (nearType != cubeType.id && cubeTypes[nearType - 1].bottom->GetMaterial().HasAlpha()))
                    {
                        hasTransparentCube = hasTransparentCube || cubeType.bottom->GetMaterial().HasAlpha();
                        VerticesPushFace(vertices, voffset, p, 3);
                        IndicesPushFace(indices[&cubeType.bottom->GetMaterial()], voffset);
                    }

                    // Back
                    nearType = 0;
                    if (z != 0)
                        nearType = cubes[cubeOffset - Common::ChunkSize2];
                    else if (chunkBack != 0)
                        nearType = GetCube(chunkBack, x, y, Common::ChunkSize - 1);
                    if (nearType == 0 || (nearType != cubeType.id && cubeTypes[nearType - 1].back->GetMaterial().HasAlpha()))
                    {
                        hasTransparentCube = hasTransparentCube || cubeType.back->GetMaterial().HasAlpha();
                        VerticesPushFace(vertices, voffset, p, 5);
                        IndicesPushFace(indices[&cubeType.back->GetMaterial()], voffset);
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
        this->_vertices->PushVertexAttribute(Tools::Renderers::DataType::Short, Tools::Renderers::VertexAttributeUsage::Position, 3); // position
        this->_vertices->PushVertexAttribute(Tools::Renderers::DataType::Short, Tools::Renderers::VertexAttributeUsage::TexCoord, 1); // Normales + Textures
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

    void ChunkMesh::Render(Tools::IRenderer& renderer, Tools::Renderers::Utils::DeferredShading& deferredShading, glm::mat4 const& world, Uint32 squaredDistance)
    {
        if (this->_triangleCount == 0)
            return;
        for (auto it = this->_meshes.begin(), ite = this->_meshes.end(); it != ite; ++it)
        {
            auto& m = it->second;
            if (m.nbIndices == 0)
                continue;
            deferredShading.RenderGeometry(*it->first,
                [this, &m, &renderer, world]()
                {
                    renderer.SetModelMatrix(world);
                    this->_vertices->Bind();
                    m.indices->Bind();
                    renderer.DrawElements(m.nbIndices);
                    m.indices->Unbind();
                    this->_vertices->Unbind();
                },
                squaredDistance);
        }
    }

}}
