#ifndef __CLIENT_RESOURCES_MqmModel_HPP__
#define __CLIENT_RESOURCES_MqmModel_HPP__

#include "tools/models/Iqm.hpp"

namespace Tools {
    class IRenderer;
    namespace Renderers {
        class ITexture2D;
        class IVertexBuffer;
        class IIndexBuffer;
    }
}

namespace Tools { namespace Models {

    class MqmModel
    {
    public:
        typedef std::function<Tools::Renderers::ITexture2D&(std::string const& name)> TextureCallback;
    public:
        struct JointInfo
        {
            JointInfo(std::string const& n, Uint32 p, glm::vec3 const& pos, glm::quat const& o, glm::vec3 const& s) :
                name(n), parent(p), position(pos), orientation(o), size(s) {}
            JointInfo() {}
            std::string name;
            Int32 parent;
            glm::vec3 position;
            glm::quat orientation;
            glm::vec3 size;
        };
        struct AnimInfo
        {
            AnimInfo(std::string const& n, Uint32 ff, Uint32 nf, float fr) :
                name(n), firstFrame(ff), numFrames(nf), frameRate(fr) {}
            AnimInfo() {}
            std::string name;
            Uint32 firstFrame;
            Uint32 numFrames;
            float frameRate;
        };
        struct FrameJoint
        {
            FrameJoint(glm::vec3 const& p, glm::quat const& o, glm::vec3 const& s) :
                position(p), orientation(o), size(s) {}
            FrameJoint() {}
            glm::vec3 position;
            glm::quat orientation;
            glm::vec3 size;
        };
    private:
        unsigned int _numTris;
        unsigned int _numVerts;
        std::vector<glm::mat4x4> _baseFrame, _inverseBaseFrame;
        std::vector<Tools::Renderers::ITexture2D*> _textures;
        Tools::Renderers::IVertexBuffer* _vertexBuffer;
        std::vector<Tools::Renderers::IIndexBuffer*> _indexBuffers;
        std::vector<Tools::Models::Iqm::Mesh> _meshes;
        std::vector<Tools::Models::Iqm::Joint> _joints;
        std::vector<JointInfo> _jointInfos;

    private:
        //anim
        std::vector<std::vector<FrameJoint>> _frames;
        std::vector<Tools::Models::Iqm::Pose> _poses;
        std::vector<Tools::Models::Iqm::Anim> _anims;
        std::vector<AnimInfo> _animInfos;

    public:
        explicit MqmModel(std::vector<char> const& data,
                TextureCallback textureCallback,
                Tools::IRenderer& renderer);
        ~MqmModel();

        void _LoadMeshes(Tools::Models::Iqm::Header const& header,
                std::vector<char> const& data,
                TextureCallback textureCallback,
                Tools::IRenderer& renderer);
        void _LoadAnimations(Tools::Models::Iqm::Header const& header, std::vector<char> const& data);

        std::vector<Tools::Models::Iqm::Mesh> const& GetMeshes() const { return this->_meshes; }
        std::vector<glm::mat4x4> const& GetInverseBindPose() const { return this->_inverseBaseFrame; }
        std::vector<glm::mat4x4> const& GetBindPose() const { return this->_baseFrame; }

        std::vector<Tools::Renderers::ITexture2D*> const& GetTextures() const { return this->_textures; }
        Tools::Renderers::IVertexBuffer* GetVertexBuffer() const { return this->_vertexBuffer; }
        std::vector<Tools::Renderers::IIndexBuffer*> const& GetIndexBuffers() const { return this->_indexBuffers; }

        std::vector<std::vector<FrameJoint>> const& GetFrames() const { return this->_frames; }
        std::vector<AnimInfo> const& GetAnimInfos() const { return this->_animInfos; }
        std::vector<JointInfo> const& GetJointInfos() const { return this->_jointInfos; }
    protected:
        // Build the bind-pose and the inverse bind-pose matrix array for the model.
        void _BuildBindPose( const std::vector<Tools::Models::Iqm::Joint>& joints );

        // Create the VBOs that are used to render the mesh with shaders
        bool _CreateVertexBuffers(
            //Tools::Models::Iqm::Triangle const* triangles,
            float const* inposition,
            float const* innormal,
            //float const* intangent,
            float const* intexcoord,
            Uint8 const* inblendindex,
            Uint8 const* inblendweight,
            Tools::IRenderer& renderer);
    };

}}

#endif