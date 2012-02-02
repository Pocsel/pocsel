
#include <iostream>

#include "resources/resources.hpp"

#include "tools/gui/Window.hpp"
#include "tools/IRenderer.hpp"
#include "tools/gui/Viewport.hpp"

class TestWidget : public Tools::Gui::Viewport
{
public:
    TestWidget() {}

    virtual void Render(Tools::IRenderer& r)
    {
        r.BeginDraw2D();

        static auto font = Tools::Gui::Font(RESOURCES_PATH "/DejaVuSerif-Italic.ttf", 16);
        r.SetModelMatrix(Tools::Matrix4<float>::CreateScale(1.5f, 1.5f, 1));
        font.Render(r, Tools::Color4f(1, 1, 1, 1), "Test string. ιΰηθ");
        
#ifdef DEBUG
        r.SetModelMatrix(Tools::Matrix4<float>::CreateTranslation(0, 36, 0));
        font.RenderTextureDebug(r);
#endif

        r.EndDraw2D();
    }

    virtual std::string const& GetName() const
    {
        static std::string name = "TestWidget";
        return name;
    }
};

int main()
{
    Tools::Gui::Window window("Renderers test");

    auto& evtMgr = window.GetEventManager();
    window.GetViewport().AddChild(std::unique_ptr<TestWidget>(new TestWidget()));
    evtMgr.Run();
}
