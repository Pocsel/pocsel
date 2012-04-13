#ifndef __CLIENT_MENU_WIDGET_BUTTON_HPP__
#define __CLIENT_MENU_WIDGET_BUTTON_HPP__

#include "tools/Vector2.hpp"
#include "tools/Matrix4.hpp"

namespace Tools {
    namespace Renderers { namespace Utils {
        class Rectangle;
    }}
    class IRenderer;
}
namespace Client {
    class ActionBinder;
    namespace Window {
        class InputManager;
    }
    namespace Menu {
        class Menu;
    }
}

namespace Client { namespace Menu { namespace Widget {

    class Button :
        private boost::noncopyable
    {
    private:
        Window::InputManager const& _inputManager;
        Menu& _menu;
        Tools::IRenderer& _renderer;
        std::function<void(void)> _callback;
        std::string _text;
        glm::fvec2 _size;
        glm::fvec2 _pos;
        bool _pressed;
        Tools::Renderers::Utils::Rectangle* _rect;
        glm::detail::tmat4x4<float> _rectMatrix;
        glm::detail::tmat4x4<float> _textMatrix;

    public:
        Button(Window::InputManager const& inputManager,
                Menu& menu,
                Tools::IRenderer& renderer,
                ActionBinder& actionBinder,
                std::function<void(void)>& callback,
                std::string const& text = "",
                glm::fvec2 const& size = glm::fvec2(50, 20),
                glm::fvec2 const& pos = glm::fvec2(0));
        ~Button();
        void Render();
        void SetText(std::string const& text);
        void SetSize(glm::fvec2 const& size);
        void SetPos(glm::fvec2 const& pos);
        std::string const& GetText() const { return this->_text; }
        glm::fvec2 const& GetSize() const { return this->_size; }
        glm::fvec2 const& GetPos() const { return this->_pos; }
    private:
        void _Update();
        void _PressedBind();
        void _ReleasedBind();
        bool _MouseIsHovering() const;
    };

}}}

#endif
