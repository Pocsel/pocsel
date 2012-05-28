#ifndef __CLIENT_GAME_MODEL_ENGINE_HPP__
#define __CLIENT_GAME_MODEL_ENGINE_HPP__

#include "tools/models/MqmModel.hpp"

namespace Client { namespace Resources {
    class ResourceManager;
}}

namespace Tools { namespace Renderers { namespace Utils { namespace Texture {
    class ITexture;
}}}}

namespace Client { namespace Game { namespace Engine {

    class ModelType;
    class Doodad;

    class Model :
        private boost::noncopyable
    {
    private:
        ModelType* _type;
        Uint32 _id;
        Uint32 _doodadId;
        Doodad* _doodad;
        Tools::Models::MqmModel const& _model;
        Tools::Models::MqmModel::AnimInfo const* _curAnimation;
        std::map<std::string, Tools::Models::MqmModel::AnimInfo> _animations;
        std::vector<glm::mat4x4> _animatedBones;
        float _animTime;
        std::vector<Tools::Renderers::Utils::Texture::ITexture*> _textures;

    public:
        Model(Resources::ResourceManager& resourceManager, Uint32 id, Uint32 doodadId, Doodad* doodad, ModelType* type);
        ~Model();

        Uint32 GetId() const { return this->_id; }
        Uint32 GetDoodadId() const { return this->_doodadId; }
        Doodad const& GetDoodad() const { return *this->_doodad; }

        void SetAnim(std::string const& anim);
        void Update(Uint32 time, float phi);

        std::vector<Tools::Models::Iqm::Mesh> const& GetMeshes() const { return this->_model.GetMeshes(); }
        std::vector<glm::mat4x4> const& GetAnimatedBones() const { return this->_animatedBones; }

        // TODO recuperer les vraies textures
        std::vector<Tools::Renderers::Utils::Texture::ITexture*> const& GetTextures() const { return this->_textures; }
        Tools::Renderers::IVertexBuffer* GetVertexBuffer() const { return this->_model.GetVertexBuffer(); }
        std::vector<Tools::Renderers::IIndexBuffer*> const& GetIndexBuffers() const { return this->_model.GetIndexBuffers(); }
    };

}}}

#endif
