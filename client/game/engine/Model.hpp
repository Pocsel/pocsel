#ifndef __CLIENT_GAME_MODEL_ENGINE_HPP__
#define __CLIENT_GAME_MODEL_ENGINE_HPP__

#include "tools/models/MqmModel.hpp"
#include "tools/renderers/utils/material/LuaMaterial.hpp"

namespace Client { namespace Resources {
    class ResourceManager;
}}

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
        std::vector<std::unique_ptr<Tools::Renderers::Utils::Material::LuaMaterial>> _materials;
        Uint32 _weakReferenceId;

    public:
        Model(Resources::ResourceManager& resourceManager, Uint32 id, Uint32 doodadId, Doodad* doodad, ModelType* type);
        ~Model();

        Uint32 GetId() const { return this->_id; }
        Uint32 GetDoodadId() const { return this->_doodadId; }
        Doodad const& GetDoodad() const { return *this->_doodad; }

        void SetAnim(std::string const& anim);
        void Update(float deltaTime, float phi); // deltaTime in second

        std::vector<Tools::Models::Iqm::Mesh> const& GetMeshes() const { return this->_model.GetMeshes(); }
        std::vector<glm::mat4x4> const& GetAnimatedBones() const { return this->_animatedBones; }

        // TODO recuperer les vraies textures
        std::vector<std::unique_ptr<Tools::Renderers::Utils::Material::LuaMaterial>> const& GetMaterials() const { return this->_materials; }
        Tools::Renderers::IVertexBuffer* GetVertexBuffer() const { return this->_model.GetVertexBuffer(); }
        std::vector<Tools::Renderers::IIndexBuffer*> const& GetIndexBuffers() const { return this->_model.GetIndexBuffers(); }
    };

}}}

#endif
