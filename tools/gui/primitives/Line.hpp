#ifndef __TOOLS_PRIMITIVES_LINE_HPP__
#define __TOOLS_PRIMITIVES_LINE_HPP__

#include <functional>

#include "IPrimitive.hpp"
#include "tools/Color.hpp"
#include "tools/Vector3.hpp"

namespace Tools { namespace Gui { namespace Primitives {

    class Line : public IPrimitive
    {
    private:
        static Renderers::IVertexBuffer* _vertexBuffer;
        static Renderers::IShaderProgram* _shader;
        static std::function<void(Line&, IRenderer&)> _renderFunc;

        Vector3<float> _point1, _point2;
        Color4f _point1Color, _point2Color;

    public:
        Line(Vector3<float> const& pt1, Vector3<float> const& pt2);

        Vector3<float> const& GetPosition1() const { return this->_point1; }
        Vector3<float> const& GetPosition2() const { return this->_point2; }
        Color4f const& GetColor1() const { return this->_point1Color; }
        Color4f const& GetColor2() const { return this->_point2Color; }

        void SetPosition(Vector3<float> const& p1, Vector3<float> const& p2) { this->_point1 = p1; this->_point2 = p2; }
        void SetColor(Color4f const& c1, Color4f const& c2) { this->_point1Color = c1; this->_point2Color = c2; }
        void SetColor(Color4f const& color) { this->SetColor(color, color); }

        virtual void Render(IRenderer& renderer) { Line::_renderFunc(*this, renderer); }

    private:
        static void _InitRender(Line& line, IRenderer& renderer);
        static void _Render(Line& line, IRenderer& renderer);
    };

}}}

#endif
