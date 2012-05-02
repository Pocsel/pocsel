#ifndef __CLIENT_GAME_MODEL_HPP__
#define __CLIENT_GAME_MODEL_HPP__

#include "client/resources/Md5Model.hpp"

namespace Client { namespace Resources {
    class LocalResourceManager;
    class Mesh;
}}

namespace Client { namespace Game {

    class Model
    {
    private:
        Resources::Md5Model const& _model;
        Resources::Md5Model::AnimInfo const* _curAnimation;
        std::map<std::string, Resources::Md5Model::AnimInfo> _animations;
        std::vector<glm::mat4x4> _animatedBones;
        float _animTime;

    public:
        Model(Resources::LocalResourceManager& resourceManager);

        void SetAnim(std::string const& anim);
        void Update(Uint32 time, float phi);

        std::vector<Tools::Iqm::Mesh> const& GetMeshes() const { return this->_model.GetMeshes(); }
        std::vector<glm::mat4x4> const& GetAnimatedBones() const { return this->_animatedBones; }

        std::vector<Tools::Renderers::ITexture2D*> const& GetTextures() const { return this->_model.GetTextures(); }
        Tools::Renderers::IVertexBuffer* GetVertexBuffer() const { return this->_model.GetVertexBuffer(); }
        std::vector<Tools::Renderers::IIndexBuffer*> const& GetIndexBuffers() const { return this->_model.GetIndexBuffers(); }
    };

}}

#endif
