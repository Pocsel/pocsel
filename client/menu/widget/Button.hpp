#ifndef __CLIENT_MENU_WIDGET_BUTTON_HPP__
#define __CLIENT_MENU_WIDGET_BUTTON_HPP__

#include "tools/Vector2.hpp"

namespace Tools {
    namespace Renderers { namespace Utils {
        class Rectangle;
    }}
    namespace Window {
        class ActionBinder;
        class InputManager;
    }
    class IRenderer;
}
namespace Client {
    namespace Menu {
        class Menu;
    }
    namespace Resources {
        class LocalResourceManager;
    }
}

namespace Client { namespace Menu { namespace Widget {

    class Button :
        private boost::noncopyable
    {
    private:
        Tools::Window::InputManager const& _inputManager;
        Menu& _menu;
        Resources::LocalResourceManager& _localResourceManager;
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
        Button(Tools::Window::InputManager const& inputManager,
                Menu& menu,
                Resources::LocalResourceManager& localResourceManager,
                Tools::Window::ActionBinder& actionBinder,
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
