#ifndef __CLIENT_GAME_BULLETDEBUGDRAWER_HPP__
#define __CLIENT_GAME_BULLETDEBUGDRAWER_HPP__

#include "bullet/bullet-all.hpp"

namespace Client { namespace Game {

    class Game;

    class BulletDebugDrawer :
        public btIDebugDraw
    {
    private:
        Game& _game;
        int _debugMode;
        btVector3 _cameraPos;

    public:
        BulletDebugDrawer(Game& game);

        void BeginDraw();
        void EndDraw();

        virtual void drawLine(const btVector3& from,const btVector3& to,const btVector3& color);
        virtual void reportErrorWarning(const char* warningString);
        virtual void draw3dText(const btVector3& location,const char* textString);
        virtual void drawContactPoint(const btVector3& PointOnB,const btVector3& normalOnB,btScalar distance,int lifeTime,const btVector3& color);
        virtual void setDebugMode(int debugMode);
        virtual int getDebugMode() const;

        virtual void drawLine(const btVector3& from,const btVector3& to,const btVector3& fromColor, const btVector3& toColor);

        //virtual void	drawLine(const btVector3& from,const btVector3& to,const btVector3& fromColor, const btVector3& toColor);
        //virtual void	drawLine(const btVector3& from,const btVector3& to,const btVector3& color);
        //virtual void	drawSphere (const btVector3& p, btScalar radius, const btVector3& color);
        //virtual void	drawBox (const btVector3& boxMin, const btVector3& boxMax, const btVector3& color, btScalar alpha);
        //virtual void	drawTriangle(const btVector3& a,const btVector3& b,const btVector3& c,const btVector3& color,btScalar alpha);
        //virtual void	drawContactPoint(const btVector3& PointOnB,const btVector3& normalOnB,btScalar distance,int lifeTime,const btVector3& color);
        //virtual void	reportErrorWarning(const char* warningString);
        //virtual void	draw3dText(const btVector3& location,const char* textString);
        //virtual void	setDebugMode(int debugMode);
        //virtual int getDebugMode() const { return m_debugMode;}
    };

}}

#endif
