#pragma once

#include "tools/gfx/IRenderer.hpp"

namespace Tools { namespace Gfx { namespace Utils {

    class Rectangle
    {
    private:
        IRenderer& _renderer;
        std::unique_ptr<IVertexBuffer> _vertexBuffer;
        std::unique_ptr<IIndexBuffer> _indexBuffer;
        glm::detail::tvec3<float> _point1, _point2, _point3, _point4;
        glm::vec4 _point1Color, _point2Color, _point3Color, _point4Color;

    public:
        Rectangle(IRenderer& renderer);
        void Render();

        glm::detail::tvec3<float> const& GetPosition1() const { return this->_point1; }
        glm::detail::tvec3<float> const& GetPosition2() const { return this->_point2; }
        glm::detail::tvec3<float> const& GetPosition3() const { return this->_point3; }
        glm::detail::tvec3<float> const& GetPosition4() const { return this->_point4; }
        glm::vec4 const& GetColor1() const { return this->_point1Color; }
        glm::vec4 const& GetColor2() const { return this->_point2Color; }
        glm::vec4 const& GetColor3() const { return this->_point3Color; }
        glm::vec4 const& GetColor4() const { return this->_point4Color; }

        void SetPosition(glm::detail::tvec3<float> const& p1, glm::detail::tvec3<float> const& p2, glm::detail::tvec3<float> const& p3, glm::detail::tvec3<float> const& p4)
        {
            this->_point1 = p1;
            this->_point2 = p2;
            this->_point3 = p3;
            this->_point4 = p4;
            this->_RefreshVertexBuffer();
        }
        void SetColor(glm::vec4 const& lt, glm::vec4 const& rt, glm::vec4 const& rb, glm::vec4 const& lb)
        {
            this->_point1Color = lt;
            this->_point2Color = rt;
            this->_point3Color = rb;
            this->_point4Color = lb;
            this->_RefreshVertexBuffer();
        }
        void SetColor(glm::vec4 const& color) { this->SetColor(color, color, color, color); }
    private:
        void _RefreshVertexBuffer();
    };

}}}
