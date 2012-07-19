#include "client/game/BulletDebugDrawer.hpp"
#include "client/game/Game.hpp"
#include "client/game/Player.hpp"

#include "client/Client.hpp"
#include "client/resources/LocalResourceManager.hpp"

#include "tools/IRenderer.hpp"

#include "tools/renderers/utils/Line.hpp"
#include "tools/renderers/utils/Cube.hpp"
#include "tools/renderers/utils/Sphere.hpp"

namespace Client { namespace Game {

    BulletDebugDrawer::BulletDebugDrawer(Game& game, Tools::IRenderer& renderer) :
        _debugMode(0),
        _game(game),
        _renderer(renderer)
    {
        this->_shader = &game.GetClient().GetLocalResourceManager().GetShader("BaseShaderColorParam.fx");
        this->_shaderColor = &this->_shader->GetParameter("colorParam");
    }

    BulletDebugDrawer::~BulletDebugDrawer()
    {
    }

    void BulletDebugDrawer::BeginDraw()
    {
        auto& camera = this->_game.GetPlayer().GetCamera();
        this->_cameraPos = btVector3(
                camera.position.x,
                camera.position.y,
                camera.position.z);

        this->_shader->BeginPass();
        this->_renderer.SetRasterizationMode(Tools::Renderers::RasterizationMode::Line);
        this->_renderer.SetCullMode(Tools::Renderers::CullMode::None);
    }

    void BulletDebugDrawer::EndDraw()
    {
        this->_renderer.SetCullMode(Tools::Renderers::CullMode::Clockwise);
        this->_renderer.SetRasterizationMode(Tools::Renderers::RasterizationMode::Fill);
        this->_shader->EndPass();
    }

    void BulletDebugDrawer::drawLine(
            const btVector3& from, const btVector3& to,
            const btVector3& fromColor, const btVector3& /*toColor*/)
    {
        if (this->_line.get() == 0)
            this->_line.reset(new Tools::Renderers::Utils::Line(this->_renderer));

        btVector3 f = from - this->_cameraPos;
        btVector3 tf = to - from;


        //Tools::Renderers::Utils::Line line(
        //        this->_renderer,
        //        glm::fvec3(0, 0, 0),
        //        glm::fvec3(tf.x(), tf.y(), tf.z()));

        this->_renderer.SetModelMatrix(
                glm::translate<float>(glm::fvec3(f.x(), f.y(), f.z()))
                );

        this->_line->Set(
                glm::fvec3(0, 0, 0),
                glm::fvec3(tf.x(), tf.y(), tf.z()));

        this->_SetColor(fromColor);
        this->_line->Render();
    }

    void BulletDebugDrawer::_SetColor(btVector3 const& fromColor)
    {
        glm::vec4 color(fromColor.x(), fromColor.y(), fromColor.z(), 1.0);
        for (unsigned int i = 0; i < 4; ++i)
        {
            if (color[i] < 0.01f)
                color[i] = 0.01f;
            if (color[i] > 0.99f)
                color[i] = 0.99f;
        }
        this->_shaderColor->Set(color);
    }

    void BulletDebugDrawer::drawLine(
            const btVector3& from, const btVector3& to,
            const btVector3& color)
    {
        this->drawLine(from, to, color, color);
    }

    void BulletDebugDrawer::drawSphere(btScalar radius, const btTransform& transform, const btVector3& color)
    {
        if (this->_sphere.get() == 0)
            this->_sphere.reset(new Tools::Renderers::Utils::Sphere(this->_renderer));

        btVector3 position = transform.getOrigin() - this->_cameraPos;
        btQuaternion rotation = transform.getRotation();

        this->_renderer.SetModelMatrix(
                glm::translate(
                    glm::fvec3(position.x(), position.y(), position.z())
                    )
                *
                glm::toMat4(
                    glm::quat((float)rotation.w(), (float)rotation.x(), (float)rotation.y(), (float)rotation.z())
                    )
                *
                glm::scale(
                    glm::fvec3((float)radius)
                    )
                );

        this->_SetColor(color);
        this->_sphere->Render();
    }

    void BulletDebugDrawer::drawSphere(const btVector3& p, btScalar radius, const btVector3& color)
    {
        btTransform tr;
        tr.setIdentity();
        tr.setOrigin(p);
        this->drawSphere(radius, tr, color);
    }

    void BulletDebugDrawer::drawBox(
            const btVector3& bbMin, const btVector3& bbMax,
            const btTransform& trans, const btVector3& color)
    {
        if (this->_cube.get() == 0)
            this->_cube.reset(new Tools::Renderers::Utils::Cube(this->_renderer));
        btVector3 extents = bbMax - bbMin;

        btVector3 position = trans.getOrigin() - this->_cameraPos;
        btQuaternion rotation = trans.getRotation();

        this->_renderer.SetModelMatrix(
                glm::translate(
                    glm::fvec3(position.x(), position.y(), position.z())
                    )
                *
                glm::toMat4(
                    glm::quat((float)rotation.w(), (float)rotation.x(), (float)rotation.y(), (float)rotation.z())
                    )
                *
                glm::scale(
                    glm::fvec3(extents.x() / 2, extents.y() / 2, extents.z() / 2)
                    )
                );

        this->_SetColor(color);
        this->_cube->Render();
    }

    void BulletDebugDrawer::drawBox(const btVector3& boxMin, const btVector3& boxMax, const btVector3& color)
    {
        if (this->_cube.get() == 0)
            this->_cube.reset(new Tools::Renderers::Utils::Cube(this->_renderer));

        btVector3 extents = boxMax - boxMin;

        btVector3 position = boxMin - this->_cameraPos;

        this->_renderer.SetModelMatrix(
                glm::translate<float>(
                    glm::fvec3(position.x(), position.y(), position.z())
                    )
                *
                glm::scale(
                    glm::fvec3(extents.x() / 2, extents.y() / 2, extents.z() / 2)
                    )
                );

        this->_SetColor(color);
        this->_cube->Render();
    }

    void BulletDebugDrawer::setDebugMode(int debugMode)
    {
        this->_debugMode = debugMode;
    }

    int BulletDebugDrawer::getDebugMode() const
    {
        return this->_debugMode;
    }

    void BulletDebugDrawer::draw3dText(const btVector3& location, const char* textString)
    {
    }

    void BulletDebugDrawer::reportErrorWarning(const char* warningString)
    {
        Tools::log << "btDebug: WARNING: " << warningString << "\n";
    }

    void BulletDebugDrawer::drawContactPoint(
            const btVector3& pointOnB, const btVector3& normalOnB,
            btScalar /*distance*/, int /*lifeTime*/, const btVector3& color)
    {
        btVector3 to = pointOnB + normalOnB * 1;
        const btVector3&from = pointOnB;

        this->drawLine(from, to, color);
    }

}}
