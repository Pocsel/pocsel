#ifndef __TOOLS_RENDERERS_UTILS_RECTANGLE_HPP__
#define __TOOLS_RENDERERS_UTILS_RECTANGLE_HPP__

#include "tools/IRenderer.hpp"

namespace Tools { namespace Renderers { namespace Utils {

    class Rectangle
    {
    private:
        IRenderer& _renderer;
        std::unique_ptr<IVertexBuffer> _vertexBuffer;
        Vector3<float> _point1, _point2, _point3, _point4;
        Color4f _point1Color, _point2Color, _point3Color, _point4Color;

    public:
        Rectangle(IRenderer& renderer);
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
            this->_RefreshVertexBuffer();
        }
        void SetColor(Color4f const& lt, Color4f const& rt, Color4f const& rb, Color4f const& lb)
        {
            this->_point1Color = lt;
            this->_point2Color = rt;
            this->_point3Color = rb;
            this->_point4Color = lb;
            this->_RefreshVertexBuffer();
        }
        void SetColor(Color4f const& color) { this->SetColor(color, color, color, color); }
    private:
        void _RefreshVertexBuffer();
    };

}}}

#endif
