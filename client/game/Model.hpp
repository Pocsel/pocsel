#ifndef __CLIENT_GAME_MODEL_HPP__
#define __CLIENT_GAME_MODEL_HPP__

#include "tools/models/MqmModel.hpp"

namespace Client { namespace Resources {
    class LocalResourceManager;
    class Mesh;
}}

namespace Client { namespace Game {

    class Model
    {
    private:
        Tools::Models::MqmModel const& _model;
        Tools::Models::MqmModel::AnimInfo const* _curAnimation;
        std::map<std::string, Tools::Models::MqmModel::AnimInfo> _animations;
        std::vector<glm::mat4x4> _animatedBones;
        float _animTime;

    public:
        Model(Resources::LocalResourceManager& resourceManager);

        void SetAnim(std::string const& anim);
        void Update(Uint32 time, float phi);

        std::vector<Tools::Models::Iqm::Mesh> const& GetMeshes() const { return this->_model.GetMeshes(); }
        std::vector<glm::mat4x4> const& GetAnimatedBones() const { return this->_animatedBones; }

        std::vector<Tools::Renderers::ITexture2D*> const& GetTextures() const { return this->_model.GetTextures(); }
        Tools::Renderers::IVertexBuffer* GetVertexBuffer() const { return this->_model.GetVertexBuffer(); }
        std::vector<Tools::Renderers::IIndexBuffer*> const& GetIndexBuffers() const { return this->_model.GetIndexBuffers(); }
    };

}}

#endif
