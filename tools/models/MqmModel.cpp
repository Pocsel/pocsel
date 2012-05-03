#include "client/precompiled.hpp"

#include "tools/models/MqmModel.hpp"

#include "tools/Math.hpp"

#include "tools/IRenderer.hpp"

namespace Tools { namespace Models {

    MqmModel::~MqmModel()
    {
        // TODO delate EVARYTHING
//        for (auto it = this->_meshes.begin(), ite = this->_meshes.end(); it != ite; ++it)
//        {
//            Tools::Delete(it->vertexBuffer);
//            Tools::Delete(it->indexBuffer);
//        }
    }

    MqmModel::MqmModel(std::vector<char> const& data,
            TextureCallback textureCallback,
            Tools::IRenderer& renderer)
    {
        Tools::Models::Iqm::Header header;

        if (data.size() < sizeof(header))
            throw std::runtime_error("MqmModel::LoadModel: data is too short, can't contain header");

        std::memcpy(&header, data.data(), sizeof(header));

        if (std::memcmp(header.magic, Tools::Models::Iqm::Magic, sizeof(header.magic)))
            throw std::runtime_error("MqmModel::LoadModel: magic is not good");

        // lilswap(&header.version, (sizeof(hdr) - sizeof(header.magic))/sizeof(uint));

        if (header.version != Tools::Models::Iqm::Version)
            throw std::runtime_error("MqmModel::LoadModel: version is not good");

        if (data.size() != header.filesize)
            throw std::runtime_error("MqmModel::LoadModel: data size is not good");

        this->_LoadMeshes(header, data, textureCallback, renderer);
        if (header.num_anims > 0)
            this->_LoadAnimations(header, data);
    }

    void MqmModel::_LoadMeshes(Tools::Models::Iqm::Header const& header,
            std::vector<char> const& data,
            TextureCallback textureCallback,
            Tools::IRenderer& renderer)
    {
        // lilswap((uint *)&buf[header.ofs_vertexarrays], header.num_vertexarrays*sizeof(iqmvertexarray)/sizeof(uint));
        // lilswap((uint *)&buf[header.ofs_triangles], header.num_triangles*sizeof(iqmtriangle)/sizeof(uint));
        // lilswap((uint *)&buf[header.ofs_meshes], header.num_meshes*sizeof(iqmmesh)/sizeof(uint));
        // lilswap((uint *)&buf[header.ofs_joints], header.num_joints*sizeof(iqmjoint)/sizeof(uint));

        //meshdata = buf;
        _numTris = header.num_triangles;
        _numVerts = header.num_vertexes;
        //_outFrame.resize(header.num_joints);
        //_textures.resize(header.num_meshes);

        _meshes.resize(header.num_meshes);
        // lilswap
        std::memcpy(_meshes.data(), data.data() + header.ofs_meshes, header.num_meshes * sizeof(_meshes[0]));
        _joints.resize(header.num_joints);
        // lilswap
        std::memcpy(_joints.data(), data.data() + header.ofs_joints, header.num_joints * sizeof(_joints[0]));

        float const *inposition = NULL, *innormal = NULL, *intangent = NULL, *intexcoord = NULL;
        Uint8 const *inblendindex = NULL, *inblendweight = NULL;
        const char *str = header.ofs_text ? &data[header.ofs_text] : "";
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
            _textures.push_back(&textureCallback(textureStr));

            this->_indexBuffers.push_back(renderer.CreateIndexBuffer().release());
            this->_indexBuffers.back()->SetData(
                    Tools::Renderers::DataType::UnsignedInt,
                    mesh.num_triangles * sizeof(Tools::Models::Iqm::Triangle), &tris[mesh.first_triangle]);

            std::cout << "mesh.first_triangle = " << mesh.first_triangle << ", num = " << mesh.num_triangles << "\n";
            std::cout << "coords " << tris[mesh.first_triangle].vertex[0] << ", " << tris[mesh.first_triangle].vertex[1] << ", " << tris[mesh.first_triangle].vertex[2] << "\n";
        }

        _CreateVertexBuffers(/*tris, */inposition, innormal, /*intangent,*/ intexcoord, inblendindex, inblendweight, renderer);

#if 0
//        if(!ebo) glGenBuffers_(1, &ebo);
//        glBindBuffer_(GL_ELEMENT_ARRAY_BUFFER, ebo);
//        glBufferData_(GL_ELEMENT_ARRAY_BUFFER, header.num_triangles*sizeof(iqmtriangle), tris, GL_STATIC_DRAW);
//        glBindBuffer_(GL_ELEMENT_ARRAY_BUFFER, 0);

//        vertex *verts = new vertex[header.num_vertexes];
//        memset(verts, 0, header.num_vertexes*sizeof(vertex));
//        for(int i = 0; i < (int)header.num_vertexes; i++)
//        {
//            vertex &v = verts[i];
//            if(inposition) memcpy(v.position, &inposition[i*3], sizeof(v.position));
//            if(innormal) memcpy(v.normal, &innormal[i*3], sizeof(v.normal));
//            if(intangent) memcpy(v.tangent, &intangent[i*4], sizeof(v.tangent));
//            if(intexcoord) memcpy(v.texcoord, &intexcoord[i*2], sizeof(v.texcoord));
//            if(inblendindex) memcpy(v.blendindex, &inblendindex[i*4], sizeof(v.blendindex));
//            if(inblendweight) memcpy(v.blendweight, &inblendweight[i*4], sizeof(v.blendweight));
//        }
//
//        if(!vbo) glGenBuffers_(1, &vbo);
//        glBindBuffer_(GL_ARRAY_BUFFER, vbo);
//        glBufferData_(GL_ARRAY_BUFFER, header.num_vertexes*sizeof(vertex), verts, GL_STATIC_DRAW);
//        glBindBuffer_(GL_ARRAY_BUFFER, 0);
//        delete[] verts;
//
//        return true;
#endif
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
        //mesh.indexBuffer = renderer.CreateIndexBuffer().release();

        std::vector<float> vertexBuffer;
//        vertexBuffer.reserve(
//                sizeof(glm::vec3) * mesh.positionBuffer.size() +
//                sizeof(glm::vec3) * mesh.normalBuffer.size() +
//                sizeof(glm::vec2) * mesh.tex2DBuffer.size() +
//                sizeof(glm::vec4) * mesh.boneWeights.size() +
//                sizeof(glm::vec4) * mesh.boneIndex.size()
//                );

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
        if(header.num_poses != header.num_joints)
            throw std::runtime_error("num_poses != num_joints");

        // lilswap((uint *)&buf[header.ofs_poses], header.num_poses*sizeof(iqmpose)/sizeof(uint));
        // lilswap((uint *)&buf[header.ofs_anims], header.num_anims*sizeof(iqmanim)/sizeof(uint));
        // lilswap((ushort *)&buf[header.ofs_frames], header.num_frames*header.num_framechannels);

        _anims.resize(header.num_anims);
        std::memcpy(_anims.data(), data.data() + header.ofs_anims, header.num_anims * sizeof(_anims[0]));

        _poses.resize(header.num_poses);
        std::memcpy(_poses.data(), data.data() + header.ofs_poses, header.num_poses * sizeof(_poses[0]));

        //_frames.resize(header.num_frames * header.num_poses);

        const char *str = header.ofs_text ? (char *)&data[header.ofs_text] : "";
        Uint16 const* framedata = (Uint16 const*)&data[header.ofs_frames];

        for(int i = 0; i < (int)header.num_frames; i++)
        {
            _frames.push_back(std::vector<FrameJoint>());
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
                // Concatenate each pose with the inverse base pose to avoid doing this at animation time.
                // If the joint has a parent, then it needs to be pre-concatenated with its parent's base pose.
                // Thus it all negates at animation time like so:
                //   (parentPose * parentInverseBasePose) * (parentBasePose * childPose * childInverseBasePose) =>
                //   parentPose * (parentInverseBasePose * parentBasePose) * childPose * childInverseBasePose =>
                //   parentPose * childPose * childInverseBasePose

                _frames.back().push_back(FrameJoint(translate, rotate, scale));

                /*
                glm::fmat4x4 m = glm::scale(scale) * glm::translate(translate) * glm::toMat4(rotate);
                if (p.parent >= 0)
                    _frames[i*header.num_poses + j] = _baseFrame[p.parent] * m * _inverseBaseFrame[j];
                else
                    _frames[i*header.num_poses + j] = m * _inverseBaseFrame[j];
                    */
            }
        }

        for (auto it = this->_anims.begin(), ite = this->_anims.end(); it != ite; ++it)
        {
            Tools::Models::Iqm::Anim &a = *it;
            this->_animInfos.push_back(AnimInfo(&str[a.name], a.first_frame, a.num_frames, a.framerate));
        }
        for (auto it = this->_joints.begin(), ite = this->_joints.end(); it != ite; ++it)
        {
            Tools::Models::Iqm::Joint &j = *it;
            this->_jointInfos.push_back(JointInfo(&str[j.name], j.parent, j.position, j.orientation, j.size));
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
