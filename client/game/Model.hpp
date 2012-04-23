#ifndef __CLIENT_GAME_MODEL_HPP__
#define __CLIENT_GAME_MODEL_HPP__

#include "client/resources/Md5Model.hpp"

namespace Client { namespace Resources {
    class Md5Model;
    class Md5Animation;
    class LocalResourceManager;
    class Mesh;
}}

namespace Client { namespace Game {

    class Model
    {
    private:
        Resources::Md5Model const& _model;
        std::map<std::string, Resources::Md5Animation const*> _animations;
        Resources::Md5Animation const* _curAnimation;
        std::vector<glm::mat4x4> _animatedBones; // Animated bone matrix from the animation with the inverse bind pose applied.
        float _animTime;

    public:
        Model(Resources::LocalResourceManager& resourceManager);

        void Update(Uint32 time, float phi);

        std::vector<Resources::Md5Model::Mesh> const& GetMeshes() const { return this->_model.GetMeshes(); }
        std::vector<glm::mat4x4> const& GetAnimatedBones() const { return this->_animatedBones; }
    };

}}

#endif
