#include "server/game/map/Chunk.hpp"

#include "bullet/bullet-all.hpp"

namespace Server { namespace Game { namespace Map {

    Chunk::Chunk(IdType id) :
        Common::BaseChunk(id),
        _shape(0),
        _body(0)
    {
    }

    Chunk::Chunk(CoordsType const& coords) :
        Common::BaseChunk(coords),
        _shape(0),
        _body(0)
    {
    }

    Chunk::~Chunk()
    {
    }

    namespace {
        inline void VerticesPushFace(std::vector<glm::dvec3>& vertices, unsigned int& offset, glm::dvec3 const& p, int idx)
        {
            static glm::dvec3 const positions[] = {
                glm::dvec3(0, 1, 1), // frontTopLeft = 0;
                glm::dvec3(1, 1, 1), // frontTopRight = 1;
                glm::dvec3(1, 1, 0), // backTopRight = 2;
                glm::dvec3(0, 1, 0), // backTopLeft = 3;
                glm::dvec3(0, 0, 0), // backBottomLeft = 4;
                glm::dvec3(1, 0, 0), // backBottomRight = 5;
                glm::dvec3(1, 0, 1), // frontBottomRight = 6;
                glm::dvec3(0, 0, 1), // frontBottomLeft = 7;
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
            vertices[offset++] = glm::dvec3(positions[*(pos++)] + p);
            vertices[offset++] = glm::dvec3(positions[*(pos++)] + p);
            vertices[offset++] = glm::dvec3(positions[*(pos++)] + p);
            vertices[offset++] = glm::dvec3(positions[*(pos++)] + p);
        }
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

    void Chunk::InitBody()
    {
        if (this->IsEmpty())
            return;

        std::vector<glm::dvec3> vertices(Common::ChunkSize3*6*4);
        std::vector<unsigned int> indices;
        unsigned int voffset = 0;

        CubeType const* cubes = this->GetCubes();
        CubeType nearType;

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

                    glm::dvec3 p((float)x, (float)y, (float)z);

                    // Right
                    if (x != Common::ChunkSize - 1)
                        nearType = cubes[cubeOffset + 1];
                    else
                        nearType = 0;
                    if (nearType == 0)
                    {
                        VerticesPushFace(vertices, voffset, p, 2);
                        IndicesPushFace(indices, voffset);
                    }

                    // Top
                    if (y != Common::ChunkSize - 1)
                        nearType = cubes[cubeOffset + Common::ChunkSize];
                    else
                        nearType = 0;
                    if (nearType == 0)
                    {
                        VerticesPushFace(vertices, voffset, p, 1);
                        IndicesPushFace(indices, voffset);
                    }

                    // Front
                    if (z != Common::ChunkSize - 1)
                        nearType = cubes[cubeOffset + Common::ChunkSize2];
                    else
                        nearType = 0;
                    if (nearType == 0)
                    {
                        VerticesPushFace(vertices,voffset,  p, 0);
                        IndicesPushFace(indices, voffset);
                    }

                    // Left
                    if (x != 0)
                        nearType = cubes[cubeOffset - 1];
                    else
                        nearType = 0;
                    if (nearType == 0)
                    {
                        VerticesPushFace(vertices, voffset, p, 4);
                        IndicesPushFace(indices, voffset);
                    }

                    // Bottom
                    if (y != 0)
                        nearType = cubes[cubeOffset - Common::ChunkSize];
                    else
                        nearType = 0;
                    if (nearType == 0)
                    {
                        VerticesPushFace(vertices, voffset, p, 3);
                        IndicesPushFace(indices, voffset);
                    }

                    // Back
                    if (z != 0)
                        nearType = cubes[cubeOffset - Common::ChunkSize2];
                    else
                        nearType = 0;
                    if (nearType == 0)
                    {
                        VerticesPushFace(vertices, voffset, p, 5);
                        IndicesPushFace(indices, voffset);
                    }
                }
            }
        }

        btTriangleIndexVertexArray* vertexArray = new btTriangleIndexVertexArray(
                indices.size() / 3,
                (int*)indices.data(),
                3*sizeof(int),
                vertices.size(),
                (double*)vertices.data(),
                sizeof(glm::dvec3)
                );

        this->_shape = new btBvhTriangleMeshShape(
                vertexArray,
                true
                );
        //btBvhTriangleMeshShape(btStridingMeshInterface* meshInterface, bool useQuantizedAabbCompression, bool buildBvh = true);

        btTransform tr;
        tr.setIdentity();
        tr.setOrigin(btVector3((this->coords.x) * 32, (this->coords.y) * 32,(this->coords.z) * 32));

        btDefaultMotionState* myMotionState = new btDefaultMotionState(tr);
        btRigidBody::btRigidBodyConstructionInfo rbInfo(0, myMotionState, this->_shape, btVector3(0, 0, 0));
        this->_body = new btRigidBody(rbInfo);

//	btTriangleIndexVertexArray(
//        int numTriangles,
//        int* triangleIndexBase,
//        int triangleIndexStride,
//        int numVertices,
//        btScalar* vertexBase,
//        int vertexStride);


    }

}}}
