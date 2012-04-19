#include "client/precompiled.hpp"

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include "client/resources/Md5Model.hpp"
#include "client/resources/LocalResourceManager.hpp"

#include "tools/Math.hpp"

#include "tools/IRenderer.hpp"

namespace Client { namespace Resources {

    Md5Model::Md5Model() :
        _md5Version(-1),
        _numJoints(0),
        _numMeshes(0),
        _hasAnimation(false),
        _localToWorldMatrix(1)
    {
    }

    Md5Model::~Md5Model()
    {
    }

    bool Md5Model::LoadModel(
            boost::filesystem::path const& filePath,
            boost::filesystem::path const& texturesPath,
            LocalResourceManager& resourceManager)
    {
        if (!boost::filesystem::exists(filePath))
        {
            Tools::error << "Md5Model::LoadModel: Failed to find file: " << filePath << "\n";
            return false;
        }

        // store the parent path used for loading images relative to this file.
        boost::filesystem::path parentPath = filePath.parent_path();

        std::string param;
        std::string junk;   // Read junk from the file

        boost::filesystem::ifstream file(filePath);
        int fileLength = (int)Tools::Filesystem::GetFileLength(file);
        if (fileLength <= 0)
        {
            Tools::error << "Md5Model::LoadModel: file " << filePath << " is empty\n";
            return false;
        }

        this->_joints.clear();
        this->_meshes.clear();

        file >> param;

        while (!file.eof())
        {
            if (param == "Md5Version")
            {
                file >> this->_md5Version;
                if (this->_md5Version != 10)
                {
                    Tools::error << "Md5Model::LoadModel: " << filePath << ": Only MD5 version 10 is supported\n";
                    return false;
                }
            }
            else if (param == "commandline")
            {
                Tools::Filesystem::IgnoreLine(file, fileLength); // Ignore the contents of the line
            }
            else if (param == "numJoints")
            {
                file >> this->_numJoints;
                this->_joints.reserve(this->_numJoints);
                this->_animatedBones.assign(this->_numJoints, glm::mat4x4(1.0f));
            }
            else if (param == "numMeshes")
            {
                file >> this->_numMeshes;
                this->_meshes.reserve(this->_numMeshes);
            }
            else if (param == "joints")
            {
                Joint joint;
                file >> junk; // Read the '{' character
                for (int i = 0; i < this->_numJoints; ++i)
                {
                    file
                        >> joint.name >> joint.parentID >> junk
                        >> joint.pos.x >> joint.pos.y >> joint.pos.z >> junk >> junk
                        >> joint.orient.x >> joint.orient.y >> joint.orient.z >> junk;

                    Tools::Filesystem::RemoveQuotes(joint.name);
                    Tools::Math::ComputeQuatW(joint.orient);

                    this->_joints.push_back(joint);
                    // Ignore everything else on the line up to the end-of-line character.
                    Tools::Filesystem::IgnoreLine(file, fileLength);
                }
                file >> junk; // Read the '}' character

                this->_BuildBindPose(this->_joints);
            }
            else if (param == "mesh")
            {
                Mesh mesh;
                int numVerts, numTris, numWeights;

                file >> junk; // Read the '{' character
                file >> param;
                while (param != "}")  // Read until we get to the '}' character
                {
                    if (param == "shader")
                    {
                        file >> mesh.shader;
                        Tools::Filesystem::RemoveQuotes(mesh.shader);

                        boost::filesystem::path shaderPath(mesh.shader);
                        boost::filesystem::path texturePath;
                        if (shaderPath.has_parent_path())
                        {
                            texturePath = shaderPath;
                        }
                        else
                        {
                            texturePath = parentPath / shaderPath;
                        }

                        //if (!texturePath.has_extension())
                        //{
                        //    texturePath.replace_extension(".tga");
                        //}

                        std::string texturePathStr = texturePath.string();
                        Tools::Filesystem::ReplaceBackslashes(texturePathStr);

                        texturePath = texturePathStr;

                        mesh.texture = &resourceManager.GetTexture2D((texturesPath / texturePath.filename()).string());

                        file.ignore(fileLength, '\n'); // Ignore everything else on the line
                    }
                    else if (param == "numverts")
                    {
                        file >> numVerts;               // Read in the vertices
                        Tools::Filesystem::IgnoreLine(file, fileLength);
                        for (int i = 0; i < numVerts; ++i)
                        {
                            Vertex vert;

                            file >> junk >> junk >> junk                    // vert vertIndex (
                                >> vert.tex0.x >> vert.tex0.y >> junk  //  s t )
                                >> vert.startWeight >> vert.weightCount;

                            Tools::Filesystem::IgnoreLine(file, fileLength);

                            vert.tex0.y = 1.0f - vert.tex0.y;
                            mesh.verts.push_back(vert);
                            mesh.tex2DBuffer.push_back(vert.tex0);
                        }
                    }
                    else if (param == "numtris")
                    {
                        file >> numTris;
                        Tools::Filesystem::IgnoreLine(file, fileLength);
                        for (int i = 0; i < numTris; ++i)
                        {
                            Triangle tri;
                            // inversion de 1 et 2 pour le cullface
                            file >> junk >> junk >> tri.indices[0] >> tri.indices[2] >> tri.indices[1];

                            Tools::Filesystem::IgnoreLine(file, fileLength);

                            mesh.tris.push_back(tri);
                            mesh.indexes.push_back((GLuint)tri.indices[0]);
                            mesh.indexes.push_back((GLuint)tri.indices[1]);
                            mesh.indexes.push_back((GLuint)tri.indices[2]);
                        }
                    }
                    else if (param == "numweights")
                    {
                        file >> numWeights;
                        Tools::Filesystem::IgnoreLine(file, fileLength);
                        for (int i = 0; i < numWeights; ++i)
                        {
                            Weight weight;
                            file >> junk >> junk >> weight.jointID >> weight.bias >> junk
                                >> weight.pos.x >> weight.pos.y >> weight.pos.z >> junk;

                            Tools::Filesystem::IgnoreLine(file, fileLength);
                            mesh.weights.push_back(weight);
                        }
                    }
                    else
                    {
                        Tools::Filesystem::IgnoreLine(file, fileLength);
                    }

                    file >> param;
                }

                this->_PrepareMesh(mesh);
                this->_PrepareNormals(mesh);
                this->_CreateVertexBuffers(mesh, resourceManager.GetRenderer());

                this->_meshes.push_back(mesh);

            }

            file >> param;
        }

        if ((int)this->_joints.size() != this->_numJoints)
        {
            Tools::error << "Md5Model::LoadModel: " << filePath <<
                ": number of joints not ok. (need " << this->_numJoints << ", has " << this->_joints.size() << ")\n";
            return false;
        }
        if ((int)this->_meshes.size() != this->_numMeshes)
        {
            Tools::error << "Md5Model::LoadModel: " << filePath <<
                ": number of meshes not ok. (need " << this->_numMeshes << ", has " << this->_meshes.size() << ")\n";
            return false;
        }

        return true;
    }

    bool Md5Model::LoadAnim(boost::filesystem::path const& filePath)
    {
        if (this->_animation.LoadAnimation(filePath))
        {
            // Check to make sure the animation is appropriate for this model
            this->_hasAnimation = this->_CheckAnimation(this->_animation);
        }

        return this->_hasAnimation;
    }

    bool Md5Model::_CheckAnimation(Md5Animation const& animation) const
    {
        if ( this->_numJoints != animation.GetNumJoints())
        {
            return false;
        }

        // Check to make sure the joints match up
        for (unsigned int i = 0; i < this->_joints.size(); ++i)
        {
            Joint const& meshJoint = this->_joints[i];
            Md5Animation::JointInfo const& animJoint = animation.GetJointInfo(i);

            if (meshJoint.name != animJoint.name || meshJoint.parentID != animJoint.parentID )
            {
                return false;
            }
        }

        return true;
    }

    void Md5Model::_BuildBindPose(std::vector<Joint> const& joints)
    {
        this->_bindPose.clear();
        this->_inverseBindPose.clear();

        for (auto it = joints.begin(), ite = joints.end(); it != ite; ++it)
        {
            Joint const& joint = (*it);

            glm::mat4x4 boneTranslation = glm::translate(joint.pos);
            glm::mat4x4 boneRotation = glm::toMat4(joint.orient);

            glm::mat4x4 boneMatrix = boneTranslation * boneRotation;

            glm::mat4x4 inverseBoneMatrix = glm::inverse(boneMatrix);

            this->_bindPose.push_back(boneMatrix);
            this->_inverseBindPose.push_back(inverseBoneMatrix);
        }
    }

    // Compute the position of the vertices in object local space
    // in the skeleton's bind pose
    bool Md5Model::_PrepareMesh(Mesh& mesh)
    {
        mesh.positionBuffer.clear();
        mesh.tex2DBuffer.clear();
        mesh.boneIndex.clear();
        mesh.boneWeights.clear();

        // Compute vertex positions
        for (auto it = mesh.verts.begin(), ite = mesh.verts.end(); it != ite; ++it)
        {
            glm::vec3 finalPos(0);
            Vertex& vert = *it;

            vert.pos = glm::vec3(0);
            vert.normal = glm::vec3(0);
            vert.boneWeights = glm::vec4(0);
            vert.boneIndices = glm::vec4(0);

            // Sum the position of the weights
            for (int j = 0; j < vert.weightCount; ++j)
            {
                Weight& weight = mesh.weights[vert.startWeight + j];
                Joint& joint = this->_joints[weight.jointID];

                // Convert the weight position from Joint local space to object space
                glm::vec3 rotPos = joint.orient * weight.pos;

                vert.pos += (joint.pos + rotPos ) * weight.bias;
                vert.boneIndices[j] = (float)weight.jointID;
                vert.boneWeights[j] = weight.bias;
            }

            mesh.positionBuffer.push_back(vert.pos);
            mesh.tex2DBuffer.push_back(vert.tex0);
            mesh.boneIndex.push_back(vert.boneIndices);
            mesh.boneWeights.push_back(vert.boneWeights);
        }

        return true;
    }

    // Compute the vertex normals in the Mesh's bind pose
    bool Md5Model::_PrepareNormals(Mesh& mesh)
    {
        mesh.normalBuffer.clear();

        // Loop through all triangles and calculate the normal of each triangle
        for (auto it = mesh.tris.begin(), ite = mesh.tris.end(); it != ite; ++it)
        {
            Triangle& tri = *it;

            glm::vec3 v0 = mesh.verts[tri.indices[0]].pos;
            glm::vec3 v1 = mesh.verts[tri.indices[1]].pos;
            glm::vec3 v2 = mesh.verts[tri.indices[2]].pos;

            glm::vec3 normal = glm::cross(v2 - v0, v1 - v0);

            mesh.verts[tri.indices[0]].normal += normal;
            mesh.verts[tri.indices[1]].normal += normal;
            mesh.verts[tri.indices[2]].normal += normal;
        }

        // Now normalize all the normals
        for (auto it = mesh.verts.begin(), ite = mesh.verts.end(); it != ite; ++it)
        {
            Vertex& vert = *it;

            vert.normal = glm::normalize(vert.normal);
            mesh.normalBuffer.push_back(vert.normal);
        }

        return true;
    }

    bool Md5Model::_PrepareMesh(Mesh& mesh, std::vector<glm::mat4x4> const& skel)
    {
        for (unsigned int i = 0; i < mesh.verts.size(); ++i)
        {
            const Vertex& vert = mesh.verts[i];
            glm::vec3& pos = mesh.positionBuffer[i];
            glm::vec3& normal = mesh.normalBuffer[i];

            pos = glm::vec3(0);
            normal = glm::vec3(0);

            for (int j = 0; j < vert.weightCount; ++j)
            {
                Weight const& weight = mesh.weights[vert.startWeight + j];
                glm::mat4x4 const& boneMatrix = skel[weight.jointID];

                pos += glm::vec3((boneMatrix * glm::vec4(vert.pos, 1.0f)) * weight.bias);
                normal += glm::vec3((boneMatrix * glm::vec4(vert.normal, 0.0f)) * weight.bias);
            }
        }
        return true;
    }

    bool Md5Model::_CreateVertexBuffers(Mesh& mesh, Tools::IRenderer& renderer)
    {
        mesh.vertexBuffer = renderer.CreateVertexBuffer().release();
        mesh.indexBuffer = renderer.CreateIndexBuffer().release();

        std::vector<float> vertexBuffer;
        vertexBuffer.reserve(
                sizeof(glm::vec3) * mesh.positionBuffer.size() +
                sizeof(glm::vec3) * mesh.normalBuffer.size() +
                sizeof(glm::vec2) * mesh.tex2DBuffer.size() +
                sizeof(glm::vec4) * mesh.boneWeights.size() +
                sizeof(glm::vec4) * mesh.boneIndex.size()
                );
        for (unsigned int i = 0; i < mesh.verts.size(); ++i)
        {
            vertexBuffer.push_back(mesh.positionBuffer[i].x);
            vertexBuffer.push_back(mesh.positionBuffer[i].y);
            vertexBuffer.push_back(mesh.positionBuffer[i].z);
            vertexBuffer.push_back(mesh.normalBuffer[i].x);
            vertexBuffer.push_back(mesh.normalBuffer[i].y);
            vertexBuffer.push_back(mesh.normalBuffer[i].z);
            vertexBuffer.push_back(mesh.tex2DBuffer[i].x);
            vertexBuffer.push_back(mesh.tex2DBuffer[i].y);
            vertexBuffer.push_back(mesh.boneWeights[i].x);
            vertexBuffer.push_back(mesh.boneWeights[i].y);
            vertexBuffer.push_back(mesh.boneWeights[i].z);
            vertexBuffer.push_back(mesh.boneWeights[i].w);
            vertexBuffer.push_back(mesh.boneIndex[i].x);
            vertexBuffer.push_back(mesh.boneIndex[i].y);
            vertexBuffer.push_back(mesh.boneIndex[i].z);
            vertexBuffer.push_back(mesh.boneIndex[i].w);
        }

        mesh.vertexBuffer->PushVertexAttribute(Tools::Renderers::DataType::Float,
                Tools::Renderers::VertexAttributeUsage::Position, 3);
        mesh.vertexBuffer->PushVertexAttribute(Tools::Renderers::DataType::Float,
                Tools::Renderers::VertexAttributeUsage::Normal, 3);
        mesh.vertexBuffer->PushVertexAttribute(Tools::Renderers::DataType::Float,
                Tools::Renderers::VertexAttributeUsage::TexCoord, 2);
        mesh.vertexBuffer->PushVertexAttribute(Tools::Renderers::DataType::Float,
                Tools::Renderers::VertexAttributeUsage::Custom1, 4);
        mesh.vertexBuffer->PushVertexAttribute(Tools::Renderers::DataType::Float,
                Tools::Renderers::VertexAttributeUsage::Custom2, 4);
        mesh.vertexBuffer->SetData(sizeof(float) * vertexBuffer.size(), vertexBuffer.data(), Tools::Renderers::VertexBufferUsage::Static);

        mesh.indexBuffer->SetData(Tools::Renderers::DataType::UnsignedInt, sizeof(GLuint) * mesh.indexes.size(), &(mesh.indexes[0]));

        return true;
    }

}}
