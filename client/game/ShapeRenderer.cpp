#include "client/game/ShapeRenderer.hpp"

#include "tools/IRenderer.hpp"
#include "tools/renderers/utils/Cube.hpp"
#include "tools/renderers/utils/Sphere.hpp"
#include "tools/window/Window.hpp"

#include "common/physics/Node.hpp"

#include "client/Client.hpp"
#include "client/game/Game.hpp"
#include "client/game/Player.hpp"
#include "client/game/engine/Body.hpp"
#include "client/resources/LocalResourceManager.hpp"

#include "bullet/bullet-all.hpp"

namespace Client { namespace Game {

    ShapeRenderer::ShapeRenderer(Game& game) :
        _game(game),
        _renderer(game.GetClient().GetWindow().GetRenderer()),
        _sphere(0),
        _cube(0)
    {
        this->_shader = &game.GetClient().GetLocalResourceManager().GetShader("CubeTarget.fx");
    }

    ShapeRenderer::~ShapeRenderer()
    {
        Tools::Delete(this->_sphere);
        Tools::Delete(this->_cube);
    }

    void ShapeRenderer::Render(Engine::Body const& body)
    {
        auto const& camera = this->_game.GetPlayer().GetCamera();


        this->_shader->BeginPass();

        this->_renderer.SetRasterizationMode(Tools::Renderers::RasterizationMode::Line);

        for (auto it = body.GetNodes().begin(), ite = body.GetNodes().end(); it != ite; ++it)
        {
            btRigidBody const& btBody = *it->body;

            btTransform bodyTr;
            btBody.getMotionState()->getWorldTransform(bodyTr);
            btQuaternion const& bodyRot = bodyTr.getRotation();
            btVector3 const& bodyPos = bodyTr.getOrigin();

            glm::quat orientation((float)bodyRot.w(), (float)bodyRot.x(), (float)bodyRot.y(), (float)bodyRot.z());
            glm::dvec3 position(bodyPos.x(), bodyPos.y(), bodyPos.z());
            glm::vec3 relPos = glm::fvec3(position - camera.position);

            const btCollisionShape* colShape = btBody.getCollisionShape();
            const btSphereShape* sphereShape;
            const btBoxShape* boxShape;

            if ((sphereShape = dynamic_cast<const btSphereShape*>(colShape)) != 0)
                this->_RenderSphere(sphereShape, orientation, relPos);
            else if ((boxShape = dynamic_cast<const btBoxShape*>(colShape)) != 0)
                this->_RenderBox(boxShape, orientation, relPos);
        }
        this->_renderer.SetRasterizationMode(Tools::Renderers::RasterizationMode::Fill);


        this->_shader->EndPass();
    }

    void ShapeRenderer::_RenderBox(
            btBoxShape const* box,
            glm::quat const& orientation,
            glm::vec3 const& pos)
    {
        //std::cout << "renderBox " << pos.x << ", " << pos.y << ", " << pos.z << " : " <<
        //    orientation.x << ", " <<
        //    orientation.y << ", " <<
        //    orientation.z << ", " <<
        //    orientation.w << "\n";
        btVector3 const& halfExtents = box->getHalfExtentsWithoutMargin();
        this->_renderer.SetModelMatrix(
                glm::translate<float>(
                    glm::fvec3(pos)
                    )
                *
                glm::toMat4(orientation)
                *
                glm::scale(
                    glm::fvec3(halfExtents.x(), halfExtents.y(), halfExtents.z())
                    )
                );

        if (!this->_cube)
            this->_cube = new Tools::Renderers::Utils::Cube(this->_renderer);
        this->_cube->Render();
    }

    void ShapeRenderer::_RenderSphere(
            btSphereShape const* sphere,
            glm::quat const& orientation,
            glm::vec3 const& pos)
    {
        //std::cout << "renderSphere " << pos.x << ", " << pos.y << ", " << pos.z << " : " <<
        //    orientation.x << ", " <<
        //    orientation.y << ", " <<
        //    orientation.z << ", " <<
        //    orientation.w << "\n";
        btScalar radius = sphere->getRadius();
        this->_renderer.SetModelMatrix(
                glm::translate<float>(
                    glm::fvec3(pos)
                    )
                *
                glm::toMat4(orientation)
                *
                glm::scale(
                    glm::fvec3(radius, radius, radius)
                    )
                );

        if (!this->_sphere)
            this->_sphere = new Tools::Renderers::Utils::Sphere(this->_renderer);
        this->_sphere->Render();
    }
}}
