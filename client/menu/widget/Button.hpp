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
        Tools::Vector2f _size;
        Tools::Vector2f _pos;
        bool _pressed;
        Tools::Renderers::Utils::Rectangle* _rect;
        Tools::Matrix4<float> _rectMatrix;
        Tools::Matrix4<float> _textMatrix;

    public:
        Button(Window::InputManager const& inputManager,
                Menu& menu,
                Tools::IRenderer& renderer,
                ActionBinder& actionBinder,
                std::function<void(void)>& callback,
                std::string const& text = "",
                Tools::Vector2f const& size = Tools::Vector2f(50, 20),
                Tools::Vector2f const& pos = Tools::Vector2f(0));
        void Render();
        void SetText(std::string const& text);
        void SetSize(Tools::Vector2f const& size);
        void SetPos(Tools::Vector2f const& pos);
        std::string const& GetText() const { return this->_text; }
        Tools::Vector2f const& GetSize() const { return this->_size; }
        Tools::Vector2f const& GetPos() const { return this->_pos; }
    private:
        void _Update();
        void _PressedBind();
        void _ReleasedBind();
        bool _MouseIsHovering() const;
    };

}}}

#endif
