#ifndef __TOOLS_RENDERERS_UTILS_RECTANGLE_HPP__
#define __TOOLS_RENDERERS_UTILS_RECTANGLE_HPP__

#include "tools/IRenderer.hpp"

namespace Tools { namespace Renderers { namespace Utils {

    class Rectangle
    {
    private:
        static IVertexBuffer* _vertexBuffer;
        static IShaderProgram* _shader;

        IRenderer& _renderer;
        Vector3<float> _point1, _point2, _point3, _point4;
        Color4f _point1Color, _point2Color, _point3Color, _point4Color;

    public:
        Rectangle(IRenderer& renderer, Vector3<float> const& pt1, Vector3<float> const& pt2, Vector3<float> const& pt3, Vector3<float> const& pt4);
        void Render();

        Vector3<float> const& GetPosition1() const { return this->_point1; }
        Vector3<float> const& GetPosition2() const { return this->_point2; }
        Vector3<float> const& GetPosition3() const { return this->_point3; }
        Vector3<float> const& GetPosition4() const { return this->_point4; }
        Color4f const& GetColor1() const { return this->_point1Color; }
        Color4f const& GetColor2() const { return this->_point2Color; }
        Color4f const& GetColor3() const { return this->_point3Color; }
        Color4f const& GetColor4() const { return this->_point4Color; }

        void SetPosition(Vector3<float> const& p1, Vector3<float> const& p2, Vector3<float> const& p3, Vector3<float> const& p4)
        {
            this->_point1 = p1;
            this->_point2 = p2;
            this->_point3 = p3;
            this->_point4 = p4;
        }
        void SetColor(Color4f const& c1, Color4f const& c2, Color4f const& c3, Color4f const& c4)
        {
            this->_point1Color = c1;
            this->_point2Color = c2;
            this->_point3Color = c3;
            this->_point4Color = c4;
        }
        void SetColor(Color4f const& color) { this->SetColor(color, color, color, color); }
    private:
        static void _InitRender(IRenderer& renderer);
    };

}}}

#endif