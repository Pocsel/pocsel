#ifndef __CLIENT_RESOURCES_Md5MODEL_HPP__
#define __CLIENT_RESOURCES_Md5MODEL_HPP__

#include "client/resources/Md5Animation.hpp"

namespace Tools { namespace Renderers {
    class ITexture2D;
}}

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

        struct Vertex
        {
            glm::vec3 pos;
            glm::vec3 normal;
            glm::vec2 tex0;
            int startWeight;
            int weightCount;
        };
        typedef std::vector<Vertex> VertexList;

        struct Triangle
        {
            int indices[3];
        };
        typedef std::vector<Triangle> TriangleList;

        struct Weight
        {
            int jointID;
            float bias;
            glm::vec3 pos;
        };
        typedef std::vector<Weight> WeightList;

        struct Joint
        {
            std::string name;
            int parentID;
            glm::vec3 pos;
            glm::quat orient;
        };
        typedef std::vector<Joint> JointList;

        struct Mesh
        {
            std::string shader;
            // This vertex list stores the vertices in the bind pose.
            VertexList verts;
            TriangleList tris;
            WeightList weights;

            // A texture ID for the material
            //GLuint texID;
            Tools::Renderers::ITexture2D* texture;

            // These buffers are used for rendering the animated mesh
            PositionBuffer positionBuffer;   // Vertex position stream
            NormalBuffer normalBuffer;     // Vertex normals stream
            Tex2DBuffer tex2DBuffer;      // Texture coordinate set
            IndexBuffer indexBuffer;      // Vertex index buffer
        };
        typedef std::vector<Mesh> MeshList;

    private:
        int _md5Version;
        int _numJoints;
        int _numMeshes;
        bool _hasAnimation;
        JointList _joints;
        MeshList _meshes;
        Md5Animation _animation;
        glm::mat4x4 _localToWorldMatrix;

    public:
        Md5Model();
        virtual ~Md5Model();

        bool LoadModel(
                boost::filesystem::path const& filePath,
                boost::filesystem::path const& texturesPath,
                LocalResourceManager& resourceManager);
        bool LoadAnim(boost::filesystem::path const& filePath);
        void Update(Uint32 time);
        void Render();

    protected:
        // Prepare the mesh for rendering
        // Compute vertex positions and normals
        bool _PrepareMesh(Mesh& mesh);
        bool _PrepareMesh(Mesh& mesh, Md5Animation::FrameSkeleton const& skel);
        bool _PrepareNormals(Mesh& mesh);

        // Render a single mesh of the model
        void _RenderMesh(Mesh const& mesh);
        void _RenderNormals(Mesh const& mesh);

        // Draw the skeleton of the mesh for debugging purposes.
        void _RenderSkeleton(JointList const& joints);

        bool _CheckAnimation(Md5Animation const& animation) const;
    };

}}

#endif
