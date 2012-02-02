#ifndef __CLIENT_GUI_MAINWINDOW_HPP__
#define __CLIENT_GUI_MAINWINDOW_HPP__

#include "tools/IRenderer.hpp"
#include "tools/gui/Viewport.hpp"

namespace Tools { namespace Gui {

    class EventManager;

    namespace Widgets {
        class Text;
    }

    namespace Events {
        struct Key;
        struct MouseMove;
        struct MouseButton;
    }

}}

namespace Client {

    class Game;

    namespace Gui {

        class GameWidget : public Tools::Gui::Viewport
        {
        private:
            Game& _game;
            Tools::Gui::EventManager& _eventManager;
            Tools::Renderers::RasterizationMode::Type _rasterMode;
            Tools::Gui::Widgets::Text* _fps;
            Tools::Gui::Widgets::Text* _renderTime;
            Tools::Gui::Widgets::Text* _triangleCount;

        public:
            GameWidget(Tools::Gui::EventManager& eventManager, Game& game);

            virtual void Render(Tools::IRenderer& r);
            virtual void OnKey(Tools::Gui::Events::Key const& e);
            virtual void OnMouseButton(Tools::Gui::Events::MouseButton const& e);
            virtual void OnMouseMove(Tools::Gui::Events::MouseMove const& e);
            virtual void OnResize(Tools::Gui::Events::Resize const& evt);

            virtual std::string const& name() const
            {
                static std::string name = "GameWidget";
                return name;
            }

        private:
            void _OnBindTest(Tools::Gui::Events::BindAction const& e);
            void _OnBindForward();
            void _OnBindBackward();
            void _OnBindLeft();
            void _OnBindRight();
            void _OnBindJump(Tools::Gui::Events::BindAction const& e);
            void _OnBindQuit(Tools::Gui::Events::BindAction const& e);
            void _OnBindToggleWireframe(Tools::Gui::Events::BindAction const& e);
        };

    }
}

#endif
