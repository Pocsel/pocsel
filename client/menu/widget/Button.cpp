#include "client/menu/widget/Button.hpp"
#include "client/ActionBinder.hpp"
#include "client/window/InputManager.hpp"
#include "client/menu/Menu.hpp"
#include "tools/renderers/utils/Rectangle.hpp"
#include "tools/renderers/utils/Font.hpp"

namespace Client { namespace Menu { namespace Widget {

    Button::Button(Window::InputManager const& inputManager,
            Menu& menu,
            Tools::IRenderer& renderer,
            ActionBinder& actionBinder,
            std::function<void(void)>& callback,
            std::string const& text /* = "" */,
            Tools::Vector2f const& size /* = Tools::Vector2f(50, 20) */,
            Tools::Vector2f const& pos /* = Tools::Vector2f(0) */) :
        _inputManager(inputManager),
        _menu(menu),
        _renderer(renderer),
        _callback(callback),
        _text(text),
        _size(size),
        _pos(pos),
        _pressed(false)
    {
        actionBinder.Bind(BindAction::MenuClick, BindAction::Pressed, std::bind(&Button::_PressedBind, this));
        actionBinder.Bind(BindAction::MenuClick, BindAction::Released, std::bind(&Button::_ReleasedBind, this));
        this->_rect = new Tools::Renderers::Utils::Rectangle(this->_renderer);
        this->_Update();
    }

    void Button::Render()
    {
        do
        {
            this->_menu.GetRectShader().BeginPass();
            this->_renderer.SetModelMatrix(this->_rectMatrix);
            this->_rect->Render();
        } while (this->_menu.GetRectShader().EndPass());
        do
        {
            this->_menu.GetFontShader().BeginPass();
            this->_renderer.SetModelMatrix(this->_textMatrix);
            this->_menu.GetFont().Render(this->_menu.GetFontTexture(), this->_text);
        } while (this->_menu.GetFontShader().EndPass());
    }

    void Button::_Update()
    {
        if (this->_pressed)
            this->_rect->SetColor(
                    Tools::Color4f(0.43f, 0.1f, 0.1f, 1),
                    Tools::Color4f(0.43f, 0.1f, 0.1f, 1),
                    Tools::Color4f(0.93f, 0.1f, 0.1f, 1),
                    Tools::Color4f(0.93f, 0.1f, 0.1f, 1));
        else
            this->_rect->SetColor(
                    Tools::Color4f(0.93f, 0.1f, 0.1f, 1),
                    Tools::Color4f(0.93f, 0.1f, 0.1f, 1),
                    Tools::Color4f(0.43f, 0.1f, 0.1f, 1),
                    Tools::Color4f(0.43f, 0.1f, 0.1f, 1));
        this->_rectMatrix = Tools::Matrix4<float>::CreateScale(this->_size.w / 2, this->_size.h / 2, 1)
            * Tools::Matrix4<float>::CreateTranslation(this->_size.w / 2 + this->_pos.w, this->_size.h / 2 + this->_pos.h, 0);
        this->_textMatrix = Tools::Matrix4<float>::CreateTranslation(this->_pos.w + 4, this->_pos.h - 3, 0);
    }

    bool Button::_MouseIsHovering() const
    {
        auto& m = this->_inputManager.GetMousePos();
        return m.x >= this->_pos.x && m.x < this->_pos.x + this->_size.x &&
            m.y >= this->_pos.y && m.y < this->_pos.y + this->_size.y;
    }

    void Button::_PressedBind()
    {
        if (this->_MouseIsHovering())
        {
            this->_pressed = true;
            this->_Update();
        }
    }

    void Button::_ReleasedBind()
    {
        this->_pressed = false;
        this->_Update();
        if (this->_MouseIsHovering())
            this->_callback();
    }

    void Button::SetText(std::string const& text)
    {
        this->_text = text;
    }

    void Button::SetSize(Tools::Vector2f const& size)
    {
        this->_size = size;
        this->_Update();
    }

    void Button::SetPos(Tools::Vector2f const& pos)
    {
        this->_pos = pos;
        this->_Update();
    }

}}}
