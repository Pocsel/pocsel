
#include <iostream>

#include "resources/resources.hpp"

#include "tools/Color.hpp"
#include "tools/IRenderer.hpp"
#include "tools/Matrix4.hpp"
#include "tools/Timer.hpp"
#include "tools/gui/Window.hpp"
#include "tools/gui/events/all.hpp"
#include "tools/gui/EventCaster.hpp"
#include "tools/gui/primitives/Cube.hpp"
#include "tools/gui/primitives/Image.hpp"
#include "tools/gui/primitives/Line.hpp"
#include "tools/gui/primitives/Plane.hpp"
#include "tools/gui/Font.hpp"
#include "tools/gui/WidgetFactory.hpp"
#include "tools/gui/widgets/Text.hpp"

#include "client/Game.hpp"
#include "client/gui/GameWidget.hpp"
#include "client/ResourceManager.hpp"
#include "client/gui/primitives/Chunk.hpp"

using namespace Tools;
using namespace Tools::Gui;
using namespace Tools::Renderers;

namespace Client { namespace Gui {

    // TODO: truc crade:
    namespace Primitives { extern int nbRenderedChunks; extern int nbChunks; extern int nbRenderedTriangles; }

    GameWidget::GameWidget(EventManager& eventManager, Game& game) :
        Viewport(),
        _game(game),
        _eventManager(eventManager),
        _rasterMode(RasterizationMode::Fill)
    {
        this->_eventManager.Connect(
            "bind/test",
            EventCaster<Events::BindAction, GameWidget>(&GameWidget::_OnBindTest, this)
        );
        this->_eventManager.Connect(
            "bind/quit",
            EventCaster<Events::BindAction, GameWidget>(&GameWidget::_OnBindQuit, this)
        );
        this->_eventManager.Connect(
            "bind/toggle-wireframe",
            EventCaster<Events::BindAction, GameWidget>(&GameWidget::_OnBindToggleWireframe, this)
        );
        this->_eventManager.Fire(
            "gamewidget/resize",
            Events::Resize(this->geometry().size.w, this->geometry().size.h)
        );
        this->_fps = &this->AddChild(WidgetFactory::Build<Widgets::Text>("FPS:"));
        this->_renderTime = &this->AddChild(WidgetFactory::Build<Widgets::Text>("ms"));
        this->_triangleCount = &this->AddChild(WidgetFactory::Build<Widgets::Text>("count"));
        this->_fps->pos().y = 20;
        this->_renderTime->pos().y = 40;
        this->_triangleCount->pos().y = 60;
    }

    void GameWidget::Render(Tools::IRenderer& r)
    {
        r.SetViewport(this->geometry());
        r.SetClearColor(Color4f(0.3f, 0.3f, 0.5f, 1));

        r.BeginDraw();

        // XXX Tests:
        static auto font = Font(RESOURCES_DIR "/DejaVuSerif-Italic.ttf", 16);
        static auto totalTimer = Tools::Timer();
        static auto updateAllTimer = Tools::Timer();
        static auto renderTimer = Tools::Timer();

        renderTimer.Reset();

        if (this->_rasterMode != RasterizationMode::Fill)
            r.SetRasterizationMode(this->_rasterMode);
        this->_game.Render(r);
        if (this->_rasterMode != RasterizationMode::Fill)
            r.SetRasterizationMode(RasterizationMode::Fill);

        Uint32 renderTime = renderTimer.GetElapsedTime();

        // XXX Bite:
        static const Common::Position pos(Common::BaseChunk::CoordsType(1 << 21, (1 << 19) + 30, 1 << 21), Vector3f(10, 0, 0));
        r.SetModelMatrix(
            Matrix4<float>::CreateScale(0.125f, 0.125f, 0.125f) *
            Matrix4<float>::CreateYawPitchRollRotation(1.57f, 3.14f, 0) *
            Matrix4<float>::CreateTranslation(pos - this->_game.GetPlayer().GetCamera().position)
        );
        Color4f c(
            std::sin(totalTimer.GetElapsedTime() * 0.0001f),
            std::cos(totalTimer.GetElapsedTime() * 0.0001f),
            std::sin(totalTimer.GetElapsedTime() * -0.0001f),
            1
        );
        font.Render(r, c, "TROLLCUBE. 0123456789ий\"за", true);

        r.EndDraw();

        Viewport::Render(r);

        Uint32 timeElapsed = updateAllTimer.GetElapsedTime();
        if (timeElapsed == 0)
            timeElapsed = 1;

        this->_fps->text("Fps: " + ToString(1000 / timeElapsed));
        this->_renderTime->text("Total: " + ToString(timeElapsed) + "ms Render game: " + ToString(renderTime) + "ms");
        this->_triangleCount->text("Chunks: " + ToString(Primitives::nbRenderedChunks) +
                                   "/" + ToString(Primitives::nbChunks) + " triangles: " +
                                   ToString(Primitives::nbRenderedTriangles));

        updateAllTimer.Reset();
    }

    void GameWidget::_OnBindTest(Events::BindAction const& e)
    {
        std::cout << "Bind \"test\" "
                  << (e.type == Events::BindAction::Type::Released ? "Released" :
                      e.type == Events::BindAction::Type::Pressed ? "Pressed" :
                      "Held")
                  << std::endl;
    }

    void GameWidget::_OnBindQuit(Events::BindAction const& e)
    {
        if (e.type == Events::BindAction::Type::Released)
            this->_eventManager.Stop();
    }

    void GameWidget::_OnBindToggleWireframe(Events::BindAction const& e)
    {
        if (e.type == Events::BindAction::Type::Pressed)
            this->_rasterMode =  (
                this->_rasterMode == RasterizationMode::Fill ?
                RasterizationMode::Line :
                (this->_rasterMode == RasterizationMode::Line ? RasterizationMode::Point : RasterizationMode::Fill)
            );
    }

    void GameWidget::OnKey(Events::Key const& /* e */)
    {
        //std::cout << "Key event [" << (e.type == Events::Key::Type::Released ? "Released" : e.type == Events::Key::Type::Pressed ? "Pressed" : "Held") << "] " <<
        //    "SDL key " << (int)e.key << ", " <<
        //    "SDL modifier " << (int)e.modifier << ", " <<
        //    "Unicode value " << (int)e.unicode << ", " <<
        //    "Ascii? " << (e.isAscii ? "Yes" : "No") << ", " <<
        //    "Ascii value '" << e.ascii << "'." << std::endl;
    }

    void GameWidget::OnMouseButton(Events::MouseButton const& /* e */)
    {
        //std::cout << "Mouse event [" << (e.type == Events::MouseButton::Type::Released ? "Released" : e.type == Events::MouseButton::Type::Pressed ? "Pressed" : "Held") << "] " <<
        //    "SDL button " << (int)e.button << ", " <<
        //    "x " << e.x << ", " <<
        //    "y " << e.y << "." << std::endl;
    }

    void GameWidget::OnMouseMove(Events::MouseMove const& e)
    {
        this->_game.GetPlayer().GetCamera().Rotate((float)-e.dx / 300.0f, (float)-e.dy / 300.0f);
    }

    void GameWidget::OnResize(Events::Resize const& evt)
    {
        Viewport::OnResize(evt);
        this->_eventManager.Fire("gamewidget/resize", evt);
    }

}}
