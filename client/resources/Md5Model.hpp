#ifndef __CLIENT_RESOURCES_MD5MODEL_HPP__
#define __CLIENT_RESOURCES_MD5MODEL_HPP__

#include "client/resources/Md5Animation.hpp"

namespace Tools {
    class IRenderer;
    namespace Renderers {
        class ITexture2D;
        class IVertexBuffer;
        class IIndexBuffer;
    }
}

namespace Client { namespace Resources {
    class LocalResourceManager;
}}

namespace Client { namespace Resources {

    class Md5Model
    {
    protected:
        typedef std::vector<glm::vec3> PositionBuffer;
        typedef std::vector<glm::vec3> NormalBuffer;
        typedef std::vector<glm::vec2> Tex2DBuffer;
        typedef std::vector<GLuint> IndexBuffer;
        typedef std::vector<glm::vec4> WeightBuffer;
        typedef std::vector<glm::vec4> BoneIndexBuffer;

        struct Vertex
        {
            glm::vec3 pos;
            glm::vec3 normal;
            glm::vec2 tex0;

            glm::vec4 boneWeights;
            glm::vec4 boneIndices;

            int startWeight;
            int weightCount;
        };

        struct Triangle
        {
            int indices[3];
        };

        struct Weight
        {
            int jointID;
            float bias;
            glm::vec3 pos;
        };

    public:
        struct Joint
        {
            std::string name;
            int parentID;
            glm::vec3 pos;
            glm::quat orient;
        };

        struct Mesh
        {
            std::string shader;
            // This vertex list stores the vertices in the bind pose.
            std::vector<Vertex> verts;
            std::vector<Triangle> tris;
            std::vector<Weight> weights;

            // A texture ID for the material
            //GLuint texID;
            Tools::Renderers::ITexture2D* texture;

            // These buffers are used for rendering the animated mesh
            PositionBuffer positionBuffer;   // Vertex position stream
            NormalBuffer normalBuffer;     // Vertex normals stream
            WeightBuffer boneWeights;      // Bone weights buffer
            BoneIndexBuffer boneIndex;       // Bone index buffer

            Tex2DBuffer tex2DBuffer;      // Texture coordinate set
            IndexBuffer indexes;      // Vertex index buffer

            Tools::Renderers::IVertexBuffer* vertexBuffer;
            Tools::Renderers::IIndexBuffer* indexBuffer;
        };

    private:
        unsigned int _md5Version;
        unsigned int _numJoints;
        unsigned int _numMeshes;
        bool _hasAnimation;
        std::vector<Joint> _joints;
        std::vector<Mesh> _meshes;
        Md5Animation _animation;
        std::vector<glm::mat4x4> _bindPose;
        std::vector<glm::mat4x4> _inverseBindPose;
        glm::mat4x4 _localToWorldMatrix;
        glm::mat4x4 _worldToLocalMatrix;

    public:
        Md5Model();
        virtual ~Md5Model();

        bool LoadModel(
                boost::filesystem::path const& filePath,
                boost::filesystem::path const& texturesPath,
                LocalResourceManager& resourceManager);
        bool LoadAnim(boost::filesystem::path const& filePath);
        void Update(Uint32 time, float phi);
        void Render(Tools::IRenderer& renderer);

        std::vector<Joint> const& GetJoints() const { return this->_joints; }
        std::vector<Mesh> const& GetMeshes() const { return this->_meshes; }
        Md5Animation const& GetAnimation() const { return this->_animation; }
        bool HasAnimation() const { return this->_hasAnimation; }
        unsigned int GetNumJoints() const { return this->_numJoints; }
        std::vector<glm::mat4x4> const& GetInverseBindPose() const { return this->_inverseBindPose; }

    protected:
        // Build the bind-pose and the inverse bind-pose matrix array for the model.
        void _BuildBindPose( const std::vector<Joint>& joints );

        // Prepare the mesh for rendering
        // Compute vertex positions and normals
        bool _PrepareMesh(Mesh& mesh);
        //bool _PrepareMesh(Mesh& mesh, Md5Animation::FrameSkeleton const& skel);
        bool _PrepareMesh(Mesh& mesh, std::vector<glm::mat4x4> const& skel);
        bool _PrepareNormals(Mesh& mesh);

        void _DestroyMesh(Mesh& mesh);

        // Create the VBOs that are used to render the mesh with shaders
        bool _CreateVertexBuffers(Mesh& mesh, Tools::IRenderer& renderer);

        // Render a single mesh of the model
        void _RenderMesh(Mesh const& mesh, Tools::IRenderer& renderer);
        void _RenderNormals(Mesh const& mesh);

        // Draw the skeleton of the mesh for debugging purposes.
        void _RenderSkeleton(std::vector<Joint> const& joints);

        bool _CheckAnimation(Md5Animation const& animation) const;
    };

}}

#endif