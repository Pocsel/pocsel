#include "client/precompiled.hpp"

#include "tools/Math.hpp"
#include "tools/renderers/utils/DeferredShading.hpp"
#include "tools/renderers/utils/material/LuaMaterial.hpp"
#include "tools/renderers/utils/material/Material.hpp"
#include "tools/renderers/utils/texture/ITexture.hpp"
#include "tools/IRenderer.hpp"
#include "tools/window/Window.hpp"
#include "tools/models/MqmModel.hpp"

#include "client/game/ModelRenderer.hpp"
#include "client/game/Game.hpp"
#include "client/game/Player.hpp"
#include "client/game/engine/Model.hpp"

#include "common/CubePosition.hpp"
#include "common/physics/Node.hpp"

#include "client/Client.hpp"

#include "client/resources/LocalResourceManager.hpp"

namespace Client { namespace Game {

    ModelRenderer::ModelRenderer(Game& game) :
        _game(game),
        _renderer(game.GetClient().GetWindow().GetRenderer())
    {
    }

    void ModelRenderer::Render(
            Tools::Renderers::Utils::DeferredShading& deferredShading,
            Engine::Model const& model,
            Common::Physics::Node const& pos,
            float updateFlag)
    {
        auto const& camera = this->_game.GetPlayer().GetCamera();

        auto const& materials =  model.GetMaterials();
        auto vertexBuffer =  model.GetVertexBuffer();
        auto const& indexBuffers =  model.GetIndexBuffers();
        auto const& meshes = model.GetMeshes();
        for (unsigned int i = 0; i < meshes.size(); ++i)
        {
            auto& var = materials[i]->GetMaterial().GetVariable<std::vector<glm::mat4x4>>("boneMatrices");
            var.Set(model.GetAnimatedBones());

            auto& varFlag = materials[i]->GetMaterial().GetVariable<float>("updateFlag");
            varFlag.Set(updateFlag);

            deferredShading.RenderGeometry(
                materials[i]->GetMaterial(),
                [&, i, vertexBuffer]()
                {
                    glm::quat orientation =
                        pos.orientation
                        *
                        glm::quat(glm::vec3(-Tools::Math::PiFloat / 2.0f, 0.0f, 0.0f))
                        ;

                    this->_renderer.SetModelMatrix(
                        glm::translate(glm::fvec3(pos.position - camera.position))
                        *
                        glm::toMat4(orientation)
                        *
                        glm::translate(glm::fvec3(0, 0, 0)) // XXX TODO pouvoir mettre orientation, pos, et scale dans RegisterModel
                    );
                    vertexBuffer->Bind();
                    indexBuffers[i]->Bind();
                    this->_renderer.DrawElements(meshes[i].num_triangles * 3);
                    indexBuffers[i]->Unbind();
                    vertexBuffer->Unbind();
                },
                Uint32(glm::lengthSquared(pos.position - camera.position)));
        }
    }

}}
