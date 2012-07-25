#ifndef __CLIENT_GAME_BULLETDEBUGDRAWER_HPP__
#define __CLIENT_GAME_BULLETDEBUGDRAWER_HPP__

#include "bullet/bullet-all.hpp"
#include "tools/stat/Timer.hpp"

namespace Tools {
    class IRenderer;
    namespace Renderers {
        class IShaderProgram;
        class IShaderParameter;
        namespace Utils {
            class Sphere;
            class Cube;
            class Line;
        }
    }
}

namespace Client { namespace Game {

    class Game;

    class BulletDebugDrawer :
        public btIDebugDraw
    {
    private:
        struct ContactPoint
        {
            ContactPoint(btVector3 const& from, btVector3 const& to, btVector3 const& color, Uint32 endTime) :
                from(from), to(to), color(color), endTime(endTime) {}
            btVector3 from;
            btVector3 to;
            btVector3 color;
            Uint32 endTime;
        };
    private:
        int _debugMode;
        btVector3 _cameraPos;

        Game& _game;
        Tools::IRenderer& _renderer;
        Tools::Renderers::IShaderProgram* _shader;
        Tools::Renderers::IShaderParameter* _shaderColor;

        std::unique_ptr<Tools::Renderers::Utils::Sphere> _sphere;
        std::unique_ptr<Tools::Renderers::Utils::Cube> _cube;
        std::unique_ptr<Tools::Renderers::Utils::Line> _line;

        Tools::Stat::Timer _timer;

        Tools::Timer _contactPointsTimer;
        Uint32 _curTime;
        std::list<ContactPoint> _contactPoints;

    public:
        BulletDebugDrawer(Game& game, Tools::IRenderer& renderer);
        ~BulletDebugDrawer();

        void BeginDraw();
        void EndDraw();

        virtual void drawLine(
                const btVector3& from, const btVector3& to,
                const btVector3& fromColor, const btVector3& toColor);
        virtual void drawLine(
                const btVector3& from, const btVector3& to,
                const btVector3& color);

        virtual void drawSphere(btScalar radius, const btTransform& transform, const btVector3& color);
        virtual void drawSphere(const btVector3& p, btScalar radius, const btVector3& color);

        //virtual void drawTriangle(
        //        const btVector3& v0, const btVector3& v1, const btVector3& v2,
        //        const btVector3& n0, const btVector3& n1, const btVector3& n2,
        //        const btVector3& color, btScalar alpha);
        //virtual void drawTriangle(
        //        const btVector3& v0, const btVector3& v1, const btVector3& v2,
        //        const btVector3& color, btScalar alpha);

        virtual void drawBox(
                const btVector3& bbMin, const btVector3& bbMax,
                const btTransform& trans, const btVector3& color);
        virtual void drawBox(
                const btVector3& bbMin, const btVector3& bbMax,
                const btVector3& color);

        //virtual void drawCapsule(
        //        btScalar radius, btScalar halfHeight, int upAxis,
        //        const btTransform& transform, const btVector3& color);
        //virtual void drawCylinder(
        //        btScalar radius, btScalar halfHeight, int upAxis,
        //        const btTransform& transform, const btVector3& color);
        //virtual void drawCone(
        //        btScalar radius, btScalar height, int upAxis,
        //        const btTransform& transform, const btVector3& color);
        //virtual void drawPlane(
        //        const btVector3& planeNormal, btScalar planeConst,
        //        const btTransform& transform, const btVector3& color);

        //virtual void drawAabb(const btVector3& from, const btVector3& to, const btVector3& color);

        //virtual void drawTransform(const btTransform& transform, btScalar orthoLen);

        //virtual void drawArc(
        //        const btVector3& center, const btVector3& normal, const btVector3& axis,
        //        btScalar radiusA, btScalar radiusB, btScalar minAngle, btScalar maxAngle,
        //        const btVector3& color, bool drawSect, btScalar stepDegrees);

        //virtual void drawSpherePatch(
        //        const btVector3& center, const btVector3& up, const btVector3& axis, btScalar radius,
        //        btScalar minTh, btScalar maxTh, btScalar minPs, btScalar maxPs,
        //        const btVector3& color, btScalar stepDegrees);

        virtual void reportErrorWarning(const char* warningString);
        virtual void draw3dText(const btVector3& location,const char* textString);
        virtual void drawContactPoint(
                const btVector3& PointOnB, const btVector3& normalOnB,
                btScalar distance, int lifeTime, const btVector3& color);
        virtual void setDebugMode(int debugMode);
        virtual int getDebugMode() const;

    private:
        void _SetColor(btVector3 const& color);
        void _ClearContactPoints();
        void _DrawContactPoints();
    };

}}

#endif
