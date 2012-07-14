#include "client/game/BulletDebugDrawer.hpp"
#include "client/game/Game.hpp"
#include "client/game/Player.hpp"

#include "client/Client.hpp"
#include "client/resources/LocalResourceManager.hpp"

#include "tools/IRenderer.hpp"

#include "tools/renderers/utils/Line.hpp"

namespace Client { namespace Game {

    BulletDebugDrawer::BulletDebugDrawer(Game& game, Tools::IRenderer& renderer) :
        _game(game),
        _renderer(renderer),
        _debugMode(0)
    {
        this->_shader = &game.GetClient().GetLocalResourceManager().GetShader("CubeTarget.fx");
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
//        this->_renderer.SetProjectionMatrix(
//                glm::mat4x4()
//                //camera.projection
//                );
//        this->_renderer.SetModelMatrix(
//                glm::mat4x4()
//                //camera.projection
//                );
                //camera.projection
//                glm::translate<float>(
//                    glm::fvec3(pos)
//                    )
//                *
                //glm::toMat4(camera.orientation)
//                *
//                glm::scale(
//                    glm::fvec3(halfExtents.x(), halfExtents.y(), halfExtents.z())
//                    )
//                );
    }

    void BulletDebugDrawer::EndDraw()
    {
        this->_renderer.SetRasterizationMode(Tools::Renderers::RasterizationMode::Fill);
        this->_shader->EndPass();
    }

    void BulletDebugDrawer::drawLine(const btVector3& from,const btVector3& to,const btVector3& fromColor, const btVector3& toColor)
    {
        btVector3 f = from - this->_cameraPos;
        btVector3 t = to - this->_cameraPos;

        btVector3 tf = t - f;

        //std::cout << "f: " <<
        //    f.x() << ", " << f.y() << ", " << f.z() << "\n";
        //std::cout << "from: " <<
        //    from.x() << ", " << from.y() << ", " << from.z() << "\n";

        Tools::Renderers::Utils::Line line(
                this->_renderer,
                glm::fvec3(0, 0, 0),//f.x(), f.y(), f.z()),
                glm::fvec3(tf.x(), tf.y(), tf.z()));
                //glm::fvec3(t.x(), t.y(), t.z()));

        this->_renderer.SetModelMatrix(
                glm::translate<float>(glm::fvec3(f.x(), f.y(), f.z()))
                //glm::mat4x4()
                //camera.projection
                );
        //glColor3f(fromColor.getX(), fromColor.getY(), fromColor.getZ());
        line.Render();
        //glColor3f(toColor.getX(), toColor.getY(), toColor.getZ());

        //glBegin(GL_LINES);
        //glColor3f(fromColor.getX(), fromColor.getY(), fromColor.getZ());
        //glVertex3d(f.getX(), f.getY(), f.getZ());
        //glColor3f(toColor.getX(), toColor.getY(), toColor.getZ());
        //glVertex3d(t.getX(), t.getY(), t.getZ());
        //glEnd();
    }

    void BulletDebugDrawer::drawLine(const btVector3& from,const btVector3& to,const btVector3& color)
    {
        this->drawLine(from, to, color, color);
    }

    void BulletDebugDrawer::setDebugMode(int debugMode)
    {
        _debugMode = debugMode;

    }

    int BulletDebugDrawer::getDebugMode() const
    {
        return this->_debugMode;
    }

    void BulletDebugDrawer::draw3dText(const btVector3& location,const char* textString)
    {
        //glRasterPos3f(location.x(),  location.y(),  location.z());
        //BMF_DrawString(BMF_GetFont(BMF_kHelvetica10),textString);
    }

    void BulletDebugDrawer::reportErrorWarning(const char* warningString)
    {
        printf("%s\n",warningString);
    }

    void BulletDebugDrawer::drawContactPoint(const btVector3& pointOnB,const btVector3& normalOnB,btScalar distance,int lifeTime,const btVector3& color)
    {

        {
            btVector3 to=pointOnB+normalOnB*1;//distance;
            const btVector3&from = pointOnB;

            this->drawLine(from, to, color);

            //           glColor4f(color.getX(), color.getY(), color.getZ(),1.f);
            //           //glColor4f(0,0,0,1.f);
            //           glBegin(GL_LINES);
            //           glVertex3d(from.getX(), from.getY(), from.getZ());
            //           glVertex3d(to.getX(), to.getY(), to.getZ());
            //           glEnd();


            //  glRasterPos3f(from.x(),  from.y(),  from.z());
            //  char buf[12];
            //  sprintf(buf," %d",lifeTime);
            //BMF_DrawString(BMF_GetFont(BMF_kHelvetica10),buf);


        }
    }
}}
