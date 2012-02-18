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
        _hasTransparentCube(false)
    {
    }

    ChunkMesh::~ChunkMesh()
    {
    }

    namespace {
        inline void IndicesPushFace(std::vector<unsigned int>& indices, unsigned int& offset)
        {
            indices.push_back(offset++);
            indices.push_back(offset++);
            indices.push_back(offset++);
            indices.push_back(offset);
            indices.push_back(offset++);
            indices.push_back(offset - 4);
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
        if (this->_chunk.IsEmpty() ||
            chunkLeft == 0 ||
            chunkRight  == 0 ||
            chunkFront  == 0 ||
            chunkBack   == 0 ||
            chunkTop    == 0 ||
            chunkBottom == 0)
            return;

        Common::BaseChunk::CubeType const* cubes = this->_chunk.GetCubes();

        Common::BaseChunk::CubeType type;
        Common::BaseChunk::CubeType nearType;

#if 1
        unsigned long x, y, z, offset, cubeOffset;
        std::map<Common::BaseChunk::CubeType, std::vector<unsigned int>> indices;
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

        for (auto it = indices.begin(), ite = indices.end(); it != ite; ++it)
        {
            if (it->second.size() == 0)
                continue;

            Mesh m;
            this->_hasTransparentCube = this->_hasTransparentCube || chunkRenderer.GetCubeInfo(it->first).isTransparent;
            m.indices = game.GetClient().GetWindow().GetRenderer().CreateIndexBuffer().release();
            m.indices->SetData(sizeof(unsigned int) * it->second.size(), it->second.data());
            m.nbIndices = (Uint32)it->second.size();
            this->_triangleCount += m.nbIndices / 3;
            this->_meshes[it->first] = std::move(m);
        }
#else
        struct Vertex
        {
            Tools::Vector3f position;
            Tools::Vector3f normal;
            Tools::Vector2f texture;
            Vertex(Tools::Vector3f const& position, Tools::Vector3f const& normal, Tools::Vector2f const& texture)
                : position(position),
                normal(normal),
                texture(texture)
            {
            }
        };
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        unsigned int offset = 0;

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
        static Tools::Vector2f const textures[] = {
            Tools::Vector2f(0, 0),
            Tools::Vector2f(1, 0),
            Tools::Vector2f(1, 1),
            Tools::Vector2f(0, 1),
        };

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

                    // Right
                    nearType = 0;
                    if (x != Common::ChunkSize - 1)
                        nearType = cubes[cubeOffset + 1];
                    else
                        nearType = chunkRight->GetCube(0, y, z);
                    if (nearType == 0 || !chunkRenderer.GetCubeInfo(nearType).isTransparent)
                    {
                        Tools::Vector3f p(x, y, z);
                        this->_hasTransparentCube = this->_hasTransparentCube || cubeType.isTransparent;
                        vertices.push_back(Vertex(positions[1] + p, normals[2], textures[0] + cubeType.right.GetXY()));
                        vertices.push_back(Vertex(positions[2] + p, normals[2], textures[3] + cubeType.right.GetXY()));
                        vertices.push_back(Vertex(positions[5] + p, normals[2], textures[2] + cubeType.right.GetXY()));
                        vertices.push_back(Vertex(positions[6] + p, normals[2], textures[1] + cubeType.right.GetXY()));
                        IndicesPushFace(indices, offset);
                    }

                    // Top
                    nearType = 0;
                    if (y != Common::ChunkSize - 1)
                        nearType = cubes[cubeOffset + Common::ChunkSize];
                    else
                        nearType = chunkTop->GetCube(x, 0, z);
                    if (nearType == 0 || !chunkRenderer.GetCubeInfo(nearType).isTransparent)
                    {
                        Tools::Vector3f p(x, y, z);
                        this->_hasTransparentCube = this->_hasTransparentCube || cubeType.isTransparent;
                        vertices.push_back(Vertex(positions[0] + p, normals[2], textures[0] + cubeType.top.GetXY()));
                        vertices.push_back(Vertex(positions[1] + p, normals[2], textures[3] + cubeType.top.GetXY()));
                        vertices.push_back(Vertex(positions[2] + p, normals[2], textures[2] + cubeType.top.GetXY()));
                        vertices.push_back(Vertex(positions[3] + p, normals[2], textures[1] + cubeType.top.GetXY()));
                        IndicesPushFace(indices, offset);
                    }

                    //if (z != Common::ChunkSize - 1)
                    //    APPEND_INDICES(cubes[cubeOffset + Common::ChunkSize2], FRONT_FACE, front);
                    //else if (z == Common::ChunkSize - 1 && chunkFront != 0)
                    //    APPEND_INDICES(chunkFront->GetCube(x, y, 0), FRONT_FACE, front);

                    //if (x != 0)
                    //    APPEND_INDICES(cubes[cubeOffset - 1], LEFT_FACE, left);
                    //else if (x == 0 && chunkLeft != 0)
                    //    APPEND_INDICES(chunkLeft->GetCube(Common::ChunkSize - 1, y, z), LEFT_FACE, left);

                    //if (y != 0)
                    //    APPEND_INDICES(cubes[cubeOffset - Common::ChunkSize], BOTTOM_FACE, bottom);
                    //else if (y == 0 && chunkBottom != 0)
                    //    APPEND_INDICES(chunkBottom->GetCube(x, Common::ChunkSize - 1, z), BOTTOM_FACE, bottom);

                    //if (z != 0)
                    //    APPEND_INDICES(cubes[cubeOffset - Common::ChunkSize2], BACK_FACE, back);
                    //else if (z == 0 && chunkBack != 0)
                    //    APPEND_INDICES(chunkBack->GetCube(x, y, Common::ChunkSize - 1), BACK_FACE, back);
                }
            }
        }

        if (indices.size() == 0)
            return;

        Mesh m;
        m.vertices = game.GetClient().GetWindow().GetRenderer().CreateVertexBuffer().release();
        m.vertices->PushVertexAttribute(Tools::Renderers::DataType::Float, Tools::Renderers::VertexAttributeUsage::Position, 3); // position
        m.vertices->PushVertexAttribute(Tools::Renderers::DataType::Float, Tools::Renderers::VertexAttributeUsage::Normal, 3); // Normales
        m.vertices->PushVertexAttribute(Tools::Renderers::DataType::Float, Tools::Renderers::VertexAttributeUsage::TexCoord, 2); // Textures
        m.vertices->SetData(vertices.size() * (3+3+2) * sizeof(float), vertices.data(), Tools::Renderers::VertexBufferUsage::Static);
        Tools::debug << this->_chunk.id << " vertices: " << vertices.size() << " (" << (vertices.size() * (3+3+2) * sizeof(float) / 1024) << "ko)\n";
        m.indices = game.GetClient().GetWindow().GetRenderer().CreateIndexBuffer().release();
        m.indices->SetData(sizeof(unsigned int) * indices.size(), indices.data());
        m.nbIndices = (Uint32)indices.size();
        this->_triangleCount += m.nbIndices / 3;
        this->_meshes[0] = std::move(m);
#endif
    }

    void ChunkMesh::Render(int cubeType, Tools::IRenderer& renderer)
    {
        Mesh& m = this->_meshes[cubeType];
        if (m.nbIndices == 0)
            return;
        //m.vertices->Bind();
        m.indices->Bind();
        renderer.DrawElements(m.nbIndices, Tools::Renderers::DataType::UnsignedInt, 0);
        m.indices->Unbind();
        //m.vertices->Unbind();
    }

}}
