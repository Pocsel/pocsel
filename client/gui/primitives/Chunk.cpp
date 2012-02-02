#include <iostream>

#include "client/gui/primitives/Chunk.hpp"
#include "client/Chunk.hpp"
#include "client/World.hpp"
#include "common/constants.hpp"
#include "common/Camera.hpp"
#include "tools/IRenderer.hpp"
#include "tools/Timer.hpp"

namespace Client { namespace Gui { namespace Primitives {

    Tools::Renderers::IVertexBuffer* Chunk::_vertexBuffer = 0;
    std::vector<Tools::Renderers::IVertexBuffer*>* Chunk::_texBuffers = 0;
    Tools::Renderers::IShaderProgram* Chunk::_shader = 0;
    Tools::Renderers::IShaderParameter* Chunk::_textureParameter = 0;
    std::vector<Tools::Renderers::ITexture2D*>* Chunk::_textures = 0;
    std::vector<Chunk::CubeInfo>* Chunk::_cubeInfos = 0;
    Tools::Timer* Chunk::_timer = 0;
    Tools::Renderers::IShaderParameter* Chunk::_timerParameter = 0;

    // TODO: truc crade pour afficher un peu de stats !
    int nbRenderedChunks = 0;
    int nbChunks = 0;
    int nbRenderedTriangles = 0;

    Chunk::MeshIndices::MeshIndices(CubeInfo const& info, Tools::Renderers::IIndexBuffer* indices, Uint32 indicesCount)
        : info(&info), indices(indices), indicesCount(indicesCount)
    {
    }
    Chunk::MeshIndices::MeshIndices(Chunk::MeshIndices&& obj) : info(obj.info), indices(obj.indices), indicesCount(obj.indicesCount)
    {
        obj.indices = 0;
    }
    Chunk::MeshIndices::~MeshIndices()
    {
        delete this->indices;
    }
    Chunk::MeshIndices& Chunk::MeshIndices::operator =(Chunk::MeshIndices&& obj)
    {
        this->info = obj.info;
        std::swap(this->indices, obj.indices);
        this->indicesCount = obj.indicesCount;
        return *this;
    }

    Chunk::Chunk(Client::Chunk& infos) :
        _infos(infos),
        _triangleCount(0),
        _hasTransparentCube(false)
    {
        ++nbChunks;
    }

    Chunk::~Chunk()
    {
        --nbChunks;
        //for (auto it = this->_indices.begin(), ite = this->_indices.end() ; it != ite ; ++it)
        //    delete *it;
    }

    void Chunk::Refresh(Tools::IRenderer& renderer, World& world)
    {
        auto chunkLeft  = world.GetChunk(this->_infos.coords + Common::BaseChunk::CoordsType(-1,  0,  0)),
            chunkRight  = world.GetChunk(this->_infos.coords + Common::BaseChunk::CoordsType( 1,  0,  0)),
            chunkFront  = world.GetChunk(this->_infos.coords + Common::BaseChunk::CoordsType( 0,  0,  1)),
            chunkBack   = world.GetChunk(this->_infos.coords + Common::BaseChunk::CoordsType( 0,  0, -1)),
            chunkTop    = world.GetChunk(this->_infos.coords + Common::BaseChunk::CoordsType( 0,  1,  0)),
            chunkBottom = world.GetChunk(this->_infos.coords + Common::BaseChunk::CoordsType( 0, -1,  0));

        if (this->_infos.IsEmpty())
        {
            this->_indices.clear();
            this->_triangleCount = 0;
            return;
        }
        Client::Chunk::CubeType const* cubes = this->_infos.GetCubes();

        unsigned long x, y, z, offset, cubeOffset;
        Client::Chunk::CubeType type;
        Client::Chunk::CubeType nearType;

        std::map<Client::Chunk::CubeType, std::vector<unsigned int>> indices;

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
#define FRONT_FACE 0
#define TOP_FACE 4
#define RIGHT_FACE 8
#define BOTTOM_FACE 12
#define LEFT_FACE 16
#define BACK_FACE 20

#define GET_INFO(cubeType)  (((*Chunk::_cubeInfos)[cubeType]))
#define APPEND_INDICES(getType, decal) \
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
                                if ((*Chunk::_cubeInfos)[type].isTransparent) \
                                    this->_hasTransparentCube = true; \
                            } \
                        } while (0)

                        if (x != Common::ChunkSize - 1)
                            APPEND_INDICES(cubes[cubeOffset + 1], RIGHT_FACE);
                        else if (x == Common::ChunkSize - 1 && chunkRight != 0)
                            APPEND_INDICES(chunkRight->GetCube(0, y, z), RIGHT_FACE);

                        if (y != Common::ChunkSize - 1)
                            APPEND_INDICES(cubes[cubeOffset + Common::ChunkSize], TOP_FACE);
                        else if (y == Common::ChunkSize - 1 && chunkTop != 0)
                            APPEND_INDICES(chunkTop->GetCube(x, 0, z), TOP_FACE);

                        if (z != Common::ChunkSize - 1)
                            APPEND_INDICES(cubes[cubeOffset + Common::ChunkSize2], FRONT_FACE);
                        else if (z == Common::ChunkSize - 1 && chunkFront != 0)
                            APPEND_INDICES(chunkFront->GetCube(x, y, 0), FRONT_FACE);

                        if (x != 0)
                            APPEND_INDICES(cubes[cubeOffset - 1], LEFT_FACE);
                        else if (x == 0 && chunkLeft != 0)
                            APPEND_INDICES(chunkLeft->GetCube(Common::ChunkSize - 1, y, z), LEFT_FACE);

                        if (y != 0)
                            APPEND_INDICES(cubes[cubeOffset - Common::ChunkSize], BOTTOM_FACE);
                        else if (y == 0 && chunkBottom != 0)
                            APPEND_INDICES(chunkBottom->GetCube(x, Common::ChunkSize - 1, z), BOTTOM_FACE);

                        if (z != 0)
                            APPEND_INDICES(cubes[cubeOffset - Common::ChunkSize2], BACK_FACE);
                        else if (z == 0 && chunkBack != 0)
                            APPEND_INDICES(chunkBack->GetCube(x, y, Common::ChunkSize - 1), BACK_FACE);

#undef APPEND_INDICES
#undef GET_INFO

                    }
                }
            }
        }

        this->_indices.clear();
        for (auto it = indices.begin(), ite = indices.end(); it != ite; ++it)
        {
            if (it->second.size() == 0)
                continue;
            auto ib = renderer.CreateIndexBuffer().release();
            ib->SetData(sizeof(unsigned int) * it->second.size(), it->second.data());
            this->_indices.push_back(MeshIndices((*Chunk::_cubeInfos)[it->first], ib, it->second.size()));
            this->_triangleCount += indices[it->first].size() / 3;
        }
    }

    void Chunk::Render(Common::Camera const& camera, Tools::IRenderer& renderer)
    {
        ++nbRenderedChunks;
        if (this->_triangleCount == 0)
            return;
        nbRenderedTriangles += this->_triangleCount;

        auto const& relativePosition = Common::Position(this->_infos.coords, Tools::Vector3f(0)) - camera.position;
        renderer.SetModelMatrix(Tools::Matrix4<float>::CreateTranslation(relativePosition));

        for (auto it = this->_indices.begin(), ite = this->_indices.end(); it != ite; ++it)
        {
            auto& texture = (*Chunk::_textures)[it->info->textureIndex];
            texture->Bind();
            it->indices->Bind();

            Chunk::_textureParameter->Set(*texture);
            renderer.DrawElements(it->indicesCount, Tools::Renderers::DataType::UnsignedInt, 0);

            it->indices->Unbind();
            texture->Unbind();
        }
    }

    void Chunk::Init(Tools::IRenderer& renderer,
            std::unique_ptr<std::vector<Tools::Renderers::ITexture2D*>>& textures,
            std::unique_ptr<std::vector<CubeInfo>>& cubeInfos)
    {
        Chunk::_textures = textures.release();
        Chunk::_cubeInfos = cubeInfos.release();

        {
            char const* vShader =
                "uniform float u_timer;"
//            "varying vec4 v_pos;"
                "varying float v_depth;"
                ""
                "void main() {"
                "   gl_TexCoord[0] = gl_MultiTexCoord0;"
//            "   v_pos = gl_ModelViewProjectionMatrix * gl_Vertex;"
//            "   gl_Position = gl_ModelViewProjectionMatrix * (gl_Vertex + "
//            "                 vec4(0, 0.01 * (cos(u_timer / 1900.0) + 1.0) * (v_pos.x * v_pos.x + v_pos.z * v_pos.z) * (1.0 + sin(gl_Vertex.y / 1245.0)), 0, 0));"
//            "   gl_Position = u_mvp * (gl_Vertex + "
//            "                 vec4(0, 0.01 * (cos(u_timer / 1900.0) + 1.0) * (v_pos.x * v_pos.x + v_pos.z * v_pos.z) * (1.0 + cos(gl_Vertex.y / 1245.0)), 0, 0));"
//            "   gl_Position = gl_ModelViewProjectionMatrix * vec4(gl_Vertex.x, gl_Vertex.y + (gl_Vertex.x * abs(cos(u_timer / 900.0)) + gl_Vertex.z * abs(cos(u_timer / 800.0))) * 0.5, gl_Vertex.z, gl_Vertex.w);"
                "   gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;"
                "   v_depth = length((gl_ModelViewMatrix * gl_Vertex).xyz);"
                "}";
            char const* fShader =
                "uniform sampler2D u_tex;"
//            "uniform float u_density = 0.00125;"
                "uniform float u_timer;"
                "varying float v_depth;"
//            "varying vec4 v_pos;"
                ""
                "void main() {"
                "   vec4 c;"
                "   float u_density = 0.00125;"

//            "   gl_FragColor = vec4(0.4 * (cos(v_pos.x * 0.02 + v_pos.y * 0.007 + 8 * cos(v_pos.y * 0.003)) + sin(v_pos.z * 0.41)),"
//            "                       sin(v_pos.y * 0.05 + v_pos.x * 0.11 - v_pos.z * 0.07),"
//            "                       abs(sin(v_pos.z * 0.03 * v_pos.y + abs(cos(1 / (1 + v_pos.x * v_pos.z * 0.001))))),"
//            "                       1);"
//            "   vec4 c = vec4(min(abs(cos(12.0 * fract(v_pos.y))),"
//            "                       abs(cos(12.0 * fract(v_pos.z)))),"
//            "                       abs(sin(v_pos.y * 0.05 + v_pos.x * 0.11 - v_pos.z * 0.07)),"
//            "                       abs(sin(v_pos.z * 0.03 * v_pos.y + abs(cos(1.0 / (1.0 + v_pos.x * v_pos.z * 0.001))))),"
//            "                       1);"

// and in the fragment shader do
                "   c = texture2D(u_tex, gl_TexCoord[0].st);"// + 0.42 * (1.0 + cos(u_timer / 700.0)));"// * 0.85 + c * 0.15;"
                "   float f = exp2( -u_density * u_density * v_depth * v_depth * 1.442695);"
                "   gl_FragColor = mix(vec4(0.8, 0.8, 0.9, 1), c, vec4(f));"
                "}";

            Chunk::_shader = renderer.CreateProgram(vShader, fShader).release();
            Chunk::_textureParameter = Chunk::_shader->GetParameter("u_tex").release();
            Chunk::_timerParameter = Chunk::_shader->GetParameter("u_timer").release();
        }

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

            float const cubeNormals[][3] = {
                {0, 0, 1},
                {0, 1, 0},
                {1, 0, 0},
                {0, -1, 0},
                {-1, 0, 0},
                {0, 0, -1}
            };

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

            unsigned int const faceIndicesN[] = {
                normalFront, normalFront, normalFront, normalFront, // front
                normalTop, normalTop, normalTop, normalTop, // top
                normalRight, normalRight, normalRight, normalRight, // right
                normalBottom, normalBottom, normalBottom, normalBottom, // bottom
                normalLeft, normalLeft, normalLeft, normalLeft, // left
                normalBack, normalBack, normalBack, normalBack, // back
            };

            float* vertices = new float[Common::ChunkSize3 * sizeof(faceIndices) / sizeof(faceIndices[0]) * 6];// * sizeof(float)];

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
                        }
                    }
                }
            }

            Chunk::_vertexBuffer = renderer.CreateVertexBuffer().release();
            Chunk::_vertexBuffer->PushVertexAttribute(Tools::Renderers::DataType::Float, Tools::Renderers::VertexAttributeUsage::Position, 3); // position
            Chunk::_vertexBuffer->PushVertexAttribute(Tools::Renderers::DataType::Float, Tools::Renderers::VertexAttributeUsage::Normal, 3); // Normales
            Chunk::_vertexBuffer->SetData(Common::ChunkSize3 * sizeof(faceIndices) / sizeof(faceIndices[0]) * 6 *sizeof(float), vertices, Tools::Renderers::VertexBufferUsage::Static);

            delete [] vertices;
        }

        {
            Chunk::_texBuffers = new std::vector<Tools::Renderers::IVertexBuffer*>();
            //    static unsigned int faceIndices[] = {
            //        /* front face */
            //        frontTopLeft, frontBottomLeft, frontBottomRight, frontTopRight,
            //        /* top face */
            //        frontTopLeft, frontTopRight, backTopRight, backTopLeft,
            //        /* right face */
            //        backTopRight, frontTopRight, frontBottomRight, backBottomRight,
            //        /* bottom face */
            //        frontBottomLeft, backBottomLeft, backBottomRight, frontBottomRight,
            //        /* left face */
            //        frontTopLeft, backTopLeft, backBottomLeft, frontBottomLeft,
            //        /* back face */
            //        backTopLeft, backTopRight, backBottomRight, backBottomLeft,
            //    };

            // les coords des textures
            float const coords[][2] = {
                {0, 1}, {0, 0}, {1, 0}, {1, 1},
                {0, 0}, {1, 0}, {1, 1}, {0, 1},
                {1, 1}, {0, 1}, {0, 0}, {1, 0},
                {0, 0}, {0, 1}, {1, 1}, {1, 0},
                {1, 1}, {0, 1}, {0, 0}, {1, 0},
                {1, 1}, {0, 1}, {0, 0}, {1, 0},
            };

            float offsets[6];

            float* vertices = new float[Common::ChunkSize3 * sizeof(coords) / sizeof(coords[0]) * 2];// * sizeof(float)];
            unsigned int x, y, z, i;
            unsigned int offset;
            float off;
            float height;
            float const* coord;
            unsigned int type;

            //for (type = 0 ; type < Chunk::_cubeInfos->size() ; ++type)
            // TODO changer ou pas changer ?
            for (type = 0 ; type < 1 ; ++type)
            {
                offsets[0] = (*Chunk::_cubeInfos)[type].side;
                offsets[1] = (*Chunk::_cubeInfos)[type].top;
                offsets[2] = (*Chunk::_cubeInfos)[type].side;
                offsets[3] = (*Chunk::_cubeInfos)[type].bottom;
                offsets[4] = (*Chunk::_cubeInfos)[type].side;
                offsets[5] = (*Chunk::_cubeInfos)[type].side;
                height = (*Chunk::_cubeInfos)[type].height;
                offset = 0;
                for (x = 0; x < Common::ChunkSize; ++x)
                {
                    for (y = 0; y < Common::ChunkSize; ++y)
                    {
                        for (z = 0; z < Common::ChunkSize; ++z)
                        {
                            for (i = 0; i < sizeof(coords) / sizeof(coords[0]); i++)
                            {
                                off = offsets[i / 4];
                                coord = coords[i];
                                vertices[offset++] = coord[0];
                                vertices[offset++] = coord[1] * height + off;
                            }
                        }
                    }
                }
                Chunk::_texBuffers->push_back(renderer.CreateVertexBuffer().release());
                (*Chunk::_texBuffers)[type]->PushVertexAttribute(Tools::Renderers::DataType::Float, Tools::Renderers::VertexAttributeUsage::TexCoord, 2); // position
                (*Chunk::_texBuffers)[type]->SetData(Common::ChunkSize3 * sizeof(coords) / sizeof(coords[0]) * 2 * sizeof(*vertices), vertices, Tools::Renderers::VertexBufferUsage::Static);
            }
            delete [] vertices;
        }

        Chunk::_timer = new Tools::Timer();
    }

    void Chunk::Bind()
    {
        Chunk::_shader->Activate();
        Chunk::_timerParameter->Set((float)((double)Chunk::_timer->GetPreciseElapsedTime() / 1000.0));
        Chunk::_vertexBuffer->Bind();
        nbRenderedChunks = 0;
        nbRenderedTriangles = 0;
        assert(Chunk::_textures->size() < 8 && "8 ca doit être le max en unité de texture, faut pas bind ici si on a plus de textures x)");
        //    for (auto it = Chunk::_textures->begin(), ite = Chunk::_textures->end(); it != ite; ++it)
        //        (*it)->Bind();
        (*Chunk::_texBuffers)[0]->Bind();
    }

    void Chunk::UnBind()
    {
        Chunk::_vertexBuffer->Unbind();
        (*Chunk::_texBuffers)[0]->Unbind();
        //    for (auto it = Chunk::_textures->begin(), ite = Chunk::_textures->end(); it != ite; ++it)
        //        (*it)->Unbind();
    }

}}}
