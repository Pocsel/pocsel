#include "client/game/BulletDebugDrawer.hpp"
#include "client/game/Game.hpp"
#include "client/game/Player.hpp"

namespace Client { namespace Game {

    BulletDebugDrawer::BulletDebugDrawer(Game& game) :
        _game(game),
        _debugMode(0)
    {
    }

    void BulletDebugDrawer::BeginDraw()
    {
        auto& camera = this->_game.GetPlayer().GetCamera();
        this->_cameraPos = btVector3(
                camera.position.x,
                camera.position.y,
                camera.position.z);
    }

    void BulletDebugDrawer::EndDraw()
    {
    }

    void BulletDebugDrawer::drawLine(const btVector3& from,const btVector3& to,const btVector3& fromColor, const btVector3& toColor)
    {
        btVector3 f = from - this->_cameraPos;
        btVector3 t = to - this->_cameraPos;
        glBegin(GL_LINES);
        glColor3f(fromColor.getX(), fromColor.getY(), fromColor.getZ());
        glVertex3d(f.getX(), f.getY(), f.getZ());
        glColor3f(toColor.getX(), toColor.getY(), toColor.getZ());
        glVertex3d(t.getX(), t.getY(), t.getZ());
        glEnd();
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
        glRasterPos3f(location.x(),  location.y(),  location.z());
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
            glColor4f(color.getX(), color.getY(), color.getZ(),1.f);
            //glColor4f(0,0,0,1.f);
            glBegin(GL_LINES);
            glVertex3d(from.getX(), from.getY(), from.getZ());
            glVertex3d(to.getX(), to.getY(), to.getZ());
            glEnd();


            //  glRasterPos3f(from.x(),  from.y(),  from.z());
            //  char buf[12];
            //  sprintf(buf," %d",lifeTime);
            //BMF_DrawString(BMF_GetFont(BMF_kHelvetica10),buf);


        }
    }
}}
