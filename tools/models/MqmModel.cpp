#include "client/precompiled.hpp"

#include "tools/models/MqmModel.hpp"

#include "tools/Math.hpp"

#include "tools/IRenderer.hpp"

namespace Tools { namespace Models {

    MqmModel::~MqmModel()
    {
        Tools::Delete(this->_vertexBuffer);
        for (auto it = this->_indexBuffers.begin(), ite = this->_indexBuffers.end(); it != ite; ++it)
            Tools::Delete(*it);
    }

    MqmModel::MqmModel(std::vector<char> const& data,
            //TextureCallback textureCallback,
            Tools::IRenderer& renderer)
    {
        Tools::debug << "MqmModel()\n";

        Tools::Models::Iqm::Header header;

        if (data.size() < sizeof(header))
            throw std::runtime_error("MqmModel:: is too short, can't contain header");

        std::memcpy(&header, data.data(), sizeof(header));

        if (std::memcmp(header.magic, Mqm::Magic, sizeof(header.magic)))
            throw std::runtime_error("MqmModel:: magic is not good");

        // lilswap(&header.version, (sizeof(hdr) - sizeof(header.magic))/sizeof(uint));

        if (header.version != Mqm::Version)
            throw std::runtime_error("MqmModel:: version is not good");

        if (data.size() != header.filesize)
            throw std::runtime_error("MqmModel:: data size is not good");

        this->_LoadMeshes(header, data, /*textureCallback,*/ renderer);
        if (header.num_anims > 0)
            this->_LoadAnimations(header, data);
    }

    void MqmModel::_LoadMeshes(Tools::Models::Iqm::Header const& header,
            std::vector<char> const& data,
            //TextureCallback textureCallback,
            Tools::IRenderer& renderer)
    {
        // lilswap((uint *)&buf[header.ofs_vertexarrays], header.num_vertexarrays*sizeof(iqmvertexarray)/sizeof(uint));
        // lilswap((uint *)&buf[header.ofs_triangles], header.num_triangles*sizeof(iqmtriangle)/sizeof(uint));
        // lilswap((uint *)&buf[header.ofs_meshes], header.num_meshes*sizeof(iqmmesh)/sizeof(uint));
        // lilswap((uint *)&buf[header.ofs_joints], header.num_joints*sizeof(iqmjoint)/sizeof(uint));
        Tools::debug << "MqmModel::LoadMeshes()\n";

        const char *str = header.ofs_text ? &data[header.ofs_text] : "";

        _numTris = header.num_triangles;
        _numVerts = header.num_vertexes;

        _meshes.resize(header.num_meshes);
        std::memcpy(_meshes.data(), data.data() + header.ofs_meshes, header.num_meshes * sizeof(_meshes[0]));
        _joints.resize(header.num_joints);
        std::memcpy(_joints.data(), data.data() + header.ofs_joints, header.num_joints * sizeof(_joints[0]));

        Tools::debug << "joints: " << header.num_joints << "\n";

        for (auto it = _joints.begin(), ite = _joints.end(); it != ite; ++it)
        {
            Tools::debug << "joint: " << &str[it->name] << "\n";
        }

        float const *inposition = NULL, *innormal = NULL, *intangent = NULL, *intexcoord = NULL;
        Uint8 const *inblendindex = NULL, *inblendweight = NULL;
        Tools::Models::Iqm::VertexArray const* vas = (Tools::Models::Iqm::VertexArray const*)&data[header.ofs_vertexarrays];
        for(int i = 0; i < (int)header.num_vertexarrays; i++)
        {
            Tools::Models::Iqm::VertexArray const& va = vas[i];
            switch(va.type)
            {
                case Tools::Models::Iqm::VertexArrayType::Position:
                    if(va.format != Tools::Models::Iqm::VertexArrayFormat::Float || va.size != 3)
                        throw std::runtime_error("fail");
                    inposition = (float const*)&data[va.offset];
                    //lilswap(inposition, 3*header.num_vertexes);
                    break;
                case Tools::Models::Iqm::VertexArrayType::Normal:
                    if(va.format != Tools::Models::Iqm::VertexArrayFormat::Float || va.size != 3)
                        throw std::runtime_error("fail");
                    innormal = (float const*)&data[va.offset];
                    //lilswap(innormal, 3*header.num_vertexes);
                    break;
                case Tools::Models::Iqm::VertexArrayType::Tangent:
                    if(va.format != Tools::Models::Iqm::VertexArrayFormat::Float || va.size != 4)
                        throw std::runtime_error("fail");
                    intangent = (float const*)&data[va.offset];
                    //lilswap(intangent, 4*header.num_vertexes);
                    break;
                case Tools::Models::Iqm::VertexArrayType::Texcoord:
                    if(va.format != Tools::Models::Iqm::VertexArrayFormat::Float || va.size != 2)
                        throw std::runtime_error("fail");
                    intexcoord = (float const*)&data[va.offset];
                    //lilswap(intexcoord, 2*header.num_vertexes);
                    break;
                case Tools::Models::Iqm::VertexArrayType::BlendIndexes:
                    if(va.format != Tools::Models::Iqm::VertexArrayFormat::Ubyte || va.size != 4)
                        throw std::runtime_error("fail");
                    inblendindex = (Uint8 const*)&data[va.offset];
                    break;
                case Tools::Models::Iqm::VertexArrayType::BlendWeights:
                    if(va.format != Tools::Models::Iqm::VertexArrayFormat::Ubyte || va.size != 4)
                        throw std::runtime_error("fail");
                    inblendweight = (Uint8 const*)&data[va.offset];
                    break;
            }
        }

        this->_BuildBindPose(_joints);

        Tools::Models::Iqm::Triangle const* tris = (Tools::Models::Iqm::Triangle const*)&data[header.ofs_triangles];

        for(int i = 0; i < (int)header.num_meshes; i++)
        {
            Tools::Models::Iqm::Mesh &mesh = _meshes[i];
            //printf("%s: loaded mesh: %s\n", filename, &str[m.name]);
            std::string textureStr(&str[mesh.material]);
            _materials.push_back(textureStr);//&textureCallback(textureStr));

            this->_indexBuffers.push_back(renderer.CreateIndexBuffer().release());
            this->_indexBuffers.back()->SetData(
                    Tools::Renderers::DataType::UnsignedInt,
                    mesh.num_triangles * sizeof(Tools::Models::Iqm::Triangle), &tris[mesh.first_triangle]);
        }

        _CreateVertexBuffers(/*tris, */inposition, innormal, /*intangent,*/ intexcoord, inblendindex, inblendweight, renderer);

        for (auto it = this->_joints.begin(), ite = this->_joints.end(); it != ite; ++it)
        {
            Tools::Models::Iqm::Joint &j = *it;
            this->_jointInfos.push_back(JointInfo(&str[j.name], j.parent, j.position, j.orientation, j.size));
        }
    }

    bool MqmModel::_CreateVertexBuffers(
            //Tools::Models::Iqm::Triangle const* triangles,
            float const* inposition,
            float const* innormal,
            //float const* intangent,
            float const* intexcoord,
            Uint8 const* inblendindex,
            Uint8 const* inblendweight,
            Tools::IRenderer& renderer)
    {
        this->_vertexBuffer = renderer.CreateVertexBuffer().release();

        std::vector<float> vertexBuffer;
        vertexBuffer.reserve(_numVerts * 3 * 3 * 2 * 4 * 4);

        for (unsigned int i = 0; i < _numVerts; ++i)
        {
            vertexBuffer.push_back(inposition[i*3+0]);
            vertexBuffer.push_back(inposition[i*3+1]);
            vertexBuffer.push_back(inposition[i*3+2]);
            vertexBuffer.push_back(innormal[i*3+0]);
            vertexBuffer.push_back(innormal[i*3+1]);
            vertexBuffer.push_back(innormal[i*3+2]);
            vertexBuffer.push_back(intexcoord[i*2+0]);
            vertexBuffer.push_back(intexcoord[i*2+1]);
            vertexBuffer.push_back((float)inblendweight[i*4+0] / (float)255);
            vertexBuffer.push_back((float)inblendweight[i*4+1] / (float)255);
            vertexBuffer.push_back((float)inblendweight[i*4+2] / (float)255);
            vertexBuffer.push_back((float)inblendweight[i*4+3] / (float)255);
            vertexBuffer.push_back((float)inblendindex[i*4+0]);
            vertexBuffer.push_back((float)inblendindex[i*4+1]);
            vertexBuffer.push_back((float)inblendindex[i*4+2]);
            vertexBuffer.push_back((float)inblendindex[i*4+3]);
        }

        this->_vertexBuffer->PushVertexAttribute(Tools::Renderers::DataType::Float,
                Tools::Renderers::VertexAttributeUsage::Position, 3);
        this->_vertexBuffer->PushVertexAttribute(Tools::Renderers::DataType::Float,
                Tools::Renderers::VertexAttributeUsage::Normal, 3);
        this->_vertexBuffer->PushVertexAttribute(Tools::Renderers::DataType::Float,
                Tools::Renderers::VertexAttributeUsage::TexCoord, 2);
        this->_vertexBuffer->PushVertexAttribute(Tools::Renderers::DataType::Float,
                Tools::Renderers::VertexAttributeUsage::Custom1, 4);
        this->_vertexBuffer->PushVertexAttribute(Tools::Renderers::DataType::Float,
                Tools::Renderers::VertexAttributeUsage::Custom2, 4);
        this->_vertexBuffer->SetData(sizeof(float) * vertexBuffer.size(), vertexBuffer.data(), Tools::Renderers::VertexBufferUsage::Static);

        //mesh.indexBuffer->SetData(Tools::Renderers::DataType::UnsignedInt, sizeof(GLuint) * mesh.indexes.size(), &(mesh.indexes[0]));

        return true;
    }

    void MqmModel::_LoadAnimations(Tools::Models::Iqm::Header const& header, std::vector<char> const& data)
    {
        if (header.num_poses != header.num_joints)
            throw std::runtime_error("num_poses != num_joints");

        // lilswap((uint *)&buf[header.ofs_poses], header.num_poses*sizeof(iqmpose)/sizeof(uint));
        // lilswap((uint *)&buf[header.ofs_anims], header.num_anims*sizeof(iqmanim)/sizeof(uint));
        // lilswap((ushort *)&buf[header.ofs_frames], header.num_frames*header.num_framechannels);

        _anims.resize(header.num_anims);
        std::memcpy(_anims.data(), data.data() + header.ofs_anims, header.num_anims * sizeof(_anims[0]));

        _poses.resize(header.num_poses);
        std::memcpy(_poses.data(), data.data() + header.ofs_poses, header.num_poses * sizeof(_poses[0]));

        _frames.reserve(header.num_frames);

        const char *str = header.ofs_text ? (char *)&data[header.ofs_text] : "";
        Uint16 const* framedata = (Uint16 const*)&data[header.ofs_frames];

        for(int i = 0; i < (int)header.num_frames; i++)
        {
            _frames.push_back(std::vector<FrameJoint>());
            _frames.back().reserve(header.num_poses);
            for(int j = 0; j < (int)header.num_poses; j++)
            {
                Tools::Models::Iqm::Pose &p = _poses[j];
                glm::quat rotate(p.channeloffset[3], p.channeloffset[4], p.channeloffset[5], p.channeloffset[6]);
                glm::vec3 translate;
                glm::vec3 scale;
                translate.x = p.channeloffset[0]; if(p.mask&0x01) translate.x += *framedata++ * p.channelscale[0];
                translate.y = p.channeloffset[1]; if(p.mask&0x02) translate.y += *framedata++ * p.channelscale[1];
                translate.z = p.channeloffset[2]; if(p.mask&0x04) translate.z += *framedata++ * p.channelscale[2];
                rotate.x = p.channeloffset[3]; if(p.mask&0x08) rotate.x += *framedata++ * p.channelscale[3];
                rotate.y = p.channeloffset[4]; if(p.mask&0x10) rotate.y += *framedata++ * p.channelscale[4];
                rotate.z = p.channeloffset[5]; if(p.mask&0x20) rotate.z += *framedata++ * p.channelscale[5];
                rotate.w = p.channeloffset[6]; if(p.mask&0x40) rotate.w += *framedata++ * p.channelscale[6];
                scale.x = p.channeloffset[7]; if(p.mask&0x80) scale.x += *framedata++ * p.channelscale[7];
                scale.y = p.channeloffset[8]; if(p.mask&0x100) scale.y += *framedata++ * p.channelscale[8];
                scale.z = p.channeloffset[9]; if(p.mask&0x200) scale.z += *framedata++ * p.channelscale[9];

                _frames.back().push_back(FrameJoint(translate, rotate, scale));
            }
        }

        for (auto it = this->_anims.begin(), ite = this->_anims.end(); it != ite; ++it)
        {
            Tools::Models::Iqm::Anim &a = *it;
            this->_animInfos.push_back(AnimInfo(&str[a.name], a.first_frame, a.num_frames, a.framerate));
            std::cout << &str[a.name] << ": framerate " << a.framerate << "\n";
        }
    }

    void MqmModel::_BuildBindPose(std::vector<Tools::Models::Iqm::Joint> const& joints)
    {
        _baseFrame.resize(joints.size());
        _inverseBaseFrame.resize(joints.size());

        for (unsigned int i = 0; i < joints.size(); ++i)
        {
            Tools::Models::Iqm::Joint const& joint = joints[i];
            _baseFrame[i] =
                glm::scale(joint.size)
                *
                glm::translate(joint.position)
                *
                glm::toMat4(glm::quat(joint.orientation));
            _inverseBaseFrame[i] = glm::inverse(_baseFrame[i]);
            if (joint.parent >= 0)
            {
                _baseFrame[i] = _baseFrame[joint.parent] * _baseFrame[i];
                _inverseBaseFrame[i] *= _inverseBaseFrame[joint.parent];
            }
        }
    }

}}
