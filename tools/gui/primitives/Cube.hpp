#ifndef __TOOLS_PRIMITIVES_CUBE_HPP__
#define __TOOLS_PRIMITIVES_CUBE_HPP__

#include "IPrimitive.hpp"

namespace Tools { namespace Gui { namespace Primitives {

    class Cube : public IPrimitive
    {
    private:
        static Renderers::IVertexBuffer* _vertexBuffer;
        static Renderers::IVertexBuffer* _colorBuffer;
        static Renderers::IShaderProgram* _shader;
        static void (*_renderFunc)(Cube&, IRenderer&);

        Color4f _point1Color, _point2Color, _point3Color, _point4Color, _point5Color, _point6Color;

    public:
        Cube();

        Color4f const& GetColor1() const { return this->_point1Color; }
        Color4f const& GetColor2() const { return this->_point2Color; }
        Color4f const& GetColor3() const { return this->_point3Color; }
        Color4f const& GetColor4() const { return this->_point4Color; }
        Color4f const& GetColor5() const { return this->_point5Color; }
        Color4f const& GetColor6() const { return this->_point6Color; }

        void SetColor(Color4f const& c1, Color4f const& c2, Color4f const& c3, Color4f const& c4, Color4f const& c5, Color4f const& c6)
        {
            this->_point1Color = c1;
            this->_point2Color = c2;
            this->_point3Color = c3;
            this->_point4Color = c4;
            this->_point5Color = c5;
            this->_point6Color = c6;
        }
        void SetColor(Color4f const& color) { this->SetColor(color, color, color, color, color, color); }

        virtual void Render(IRenderer& renderer) { Cube::_renderFunc(*this, renderer); }

    private:
        static void _InitRender(Cube& cube, IRenderer& renderer);
        static void _Render(Cube& cube, IRenderer& renderer);
    };

}}}

#endif
