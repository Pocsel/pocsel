#include "tests/shaders/precompiled.hpp"

#include "common/ConfDir.hpp"

#include "tools/gfx/effect/EffectManager.hpp"
#include "tools/gfx/utils/Font.hpp"
#include "tools/gfx/utils/Image.hpp"
#include "tools/gfx/IRenderer.hpp"
#include "tools/window/InputManager.hpp"
#include "tools/window/sdl/Window.hpp"
#include "tools/Frustum.hpp"
#include "tools/Timer.hpp"
#include "tools/Vector2.hpp"
#include "tools/Vector3.hpp"

using namespace Tools::Gfx;
using namespace Tools::Gfx::Utils;
using namespace Tools::Window;

#undef main

static std::string testShaderPath = "test.fx";
//static std::string directionnalLightShaderPath = "DirectionnalLight.fx";
//static std::string pointLightShaderPath = "DirectionnalLight.fx";//"PointLight.fx";
//static std::string combineShaderPath = "combine.fx";
//static std::string texturePath = "test.png";

static std::string codeTestShader;

struct RAII
{
    std::function<void(void)> destroy;
    RAII(std::function<void(void)> destroy) : destroy(destroy) {}
    ~RAII() { destroy(); }
};

struct Vertex
{
    glm::fvec3 position;
    glm::fvec3 normal;
    glm::fvec2 texture;
    Vertex() {}
    Vertex(glm::fvec3 const& position, glm::fvec3 const& normal, glm::fvec2 const& texture) : position(position), normal(normal), texture(texture) {}
    Vertex(float px, float py, float pz, float nx, float ny, float nz, float tx, float ty) : position(px, py, pz), normal(nx, ny, nz), texture(tx, ty) {}
};

struct Cube
{
    IRenderer& renderer;
    std::unique_ptr<IVertexBuffer> vertexBuffer;
    std::unique_ptr<IIndexBuffer> indexBuffer;

    Cube(IRenderer& renderer) :
        renderer(renderer)
    {
        vertexBuffer = renderer.CreateVertexBuffer();
        indexBuffer = renderer.CreateIndexBuffer();

        static Vertex vertices[] = {
            Vertex( 0.5f, -0.5f,  0.5f,    0,  0,  1,   0, 0), Vertex( 0.5f,  0.5f,  0.5f,    0,  0,  1,   0, 1),
            Vertex(-0.5f,  0.5f,  0.5f,    0,  0,  1,   1, 1), Vertex(-0.5f, -0.5f,  0.5f,    0,  0,  1,   1, 0),

            Vertex(-0.5f,  0.5f,  0.5f,    0,  1,  0,   0, 0), Vertex( 0.5f,  0.5f,  0.5f,    0,  1,  0,   0, 1),
            Vertex( 0.5f,  0.5f, -0.5f,    0,  1,  0,   1, 1), Vertex(-0.5f,  0.5f, -0.5f,    0,  1,  0,   1, 0),

            Vertex( 0.5f, -0.5f, -0.5f,    1,  0,  0,   0, 0), Vertex( 0.5f,  0.5f, -0.5f,    1,  0,  0,   0, 1),
            Vertex( 0.5f,  0.5f,  0.5f,    1,  0,  0,   1, 1), Vertex( 0.5f, -0.5f,  0.5f,    1,  0,  0,   1, 0),

            Vertex(-0.5f, -0.5f, -0.5f,    0, -1,  0,   0, 0), Vertex( 0.5f, -0.5f, -0.5f,    0, -1,  0,   0, 1),
            Vertex( 0.5f, -0.5f,  0.5f,    0, -1,  0,   1, 1), Vertex(-0.5f, -0.5f,  0.5f,    0, -1,  0,   1, 0),

            Vertex(-0.5f, -0.5f,  0.5f,   -1,  0,  0,   0, 0), Vertex(-0.5f,  0.5f,  0.5f,   -1,  0,  0,   0, 1),
            Vertex(-0.5f,  0.5f, -0.5f,   -1,  0,  0,   1, 1), Vertex(-0.5f, -0.5f, -0.5f,   -1,  0,  0,   1, 0),

            Vertex(-0.5f, -0.5f, -0.5f,    0,  0, -1,   0, 0), Vertex(-0.5f,  0.5f, -0.5f,    0,  0, -1,   0, 1),
            Vertex( 0.5f,  0.5f, -0.5f,    0,  0, -1,   1, 1), Vertex( 0.5f, -0.5f, -0.5f,    0,  0, -1,   1, 0),
        };

        static unsigned short indices[] = {
             0,  1,  2,   0,  2,  3,     4,  5,  6,   4,  6,  7,
             8,  9, 10,   8, 10, 11,    12, 13, 14,  12, 14, 15,
            16, 17, 18,  16, 18, 19,    20, 21, 22,  20, 22, 23,
        };

        vertexBuffer->PushVertexAttribute(DataType::Float, VertexAttributeUsage::Position, 3);
        vertexBuffer->PushVertexAttribute(DataType::Float, VertexAttributeUsage::Normal, 3);
        vertexBuffer->PushVertexAttribute(DataType::Float, VertexAttributeUsage::TexCoord0, 2);
        vertexBuffer->SetData(sizeof(vertices), vertices, VertexBufferUsage::Static);

        indexBuffer->SetData(DataType::UnsignedShort, sizeof(indices), indices);
    }

    void Render()
    {
        vertexBuffer->Bind();
        indexBuffer->Bind();

        renderer.DrawElements(6*3*2, DataType::UnsignedShort);

        indexBuffer->Unbind();
        vertexBuffer->Unbind();
    }
};

//static std::string ReadFile(std::string const& filename)
//{
//    std::ifstream file(filename);
//    return std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
//}
//
//static std::unique_ptr<IShaderProgram> LoadShader(IRenderer& renderer, std::string const& oldShader, std::string& shaderCode)
//{
//    try
//    {
//        return renderer.CreateProgram(shaderCode);
//    }
//    catch (std::exception& ex)
//    {
//        Tools::error << ex.what() << std::endl;
//        shaderCode = oldShader;
//        return renderer.CreateProgram(shaderCode);
//    }
//}

int main(int ac, char *av[])
{
    if (ac > 1)
        testShaderPath = av[1];
    //if (ac > 2)
    //    directionnalLightShaderPath = av[2];
    //if (ac > 3)
    //    combineShaderPath = av[3];
    //if (ac > 4)
    //    texturePath = av[4];

    Tools::log << "Current directory " << boost::filesystem::current_path() << std::endl;
    //Tools::log << "Loading \"" << testShaderPath + "\", \"" << combineShaderPath << "\", \"" << texturePath << "\"\n";

    std::map<std::string, Tools::Window::BindAction::BindAction> actions;
    actions["quit"] = BindAction::Quit;
    actions["reloadshaders"] = BindAction::ReloadShaders;
    actions["togglecullface"] = BindAction::ToggleCullface;

    Sdl::Window window(actions, false, glm::uvec2(1024));
    IRenderer& renderer = window.GetRenderer();
    bool run = true;
    bool reload = true;
    bool cullface = true;

    window.GetInputManager().Bind(BindAction::ReloadShaders, BindAction::Released, [&]() { reload = true; });
    window.GetInputManager().Bind(BindAction::Quit, BindAction::Released, [&]() { run = false; Tools::log << "Quit\n"; });
    window.GetInputManager().Bind(BindAction::ToggleCullface, BindAction::Released, [&]() { cullface = !cullface; });

    window.GetInputManager().GetInputBinder().Bind("r", "reloadshaders");
    window.GetInputManager().GetInputBinder().Bind("escape", "quit");
    window.GetInputManager().GetInputBinder().Bind("f", "togglecullface");
    //window.RegisterCallback([](glm::uvec2 const& size) { if (gbuffer) gbuffer->Resize(size); });

    {
        using Tools::Gfx::Effect::EffectManager;

        EffectManager effectMgr(renderer);

        Tools::Timer timer;
        Cube cube(renderer);

        auto texture = renderer.CreateTexture2D("test.png");
        //auto simpleShader = LoadShader(renderer, "", ReadFile("BaseShaderTexture.fx"));
        //auto& simpleBaseTex = simpleShader->GetParameter("baseTex");

        //std::string shaderCode = ReadFile(testShaderPath);
        //auto shader = LoadShader(renderer, "", shaderCode);

        //auto& shaderDiffuse = shader->GetParameter("diffuse");
        //shaderDiffuse.Set(*texture);

        //Tools::debug << "Useable baseTex, diffuse ? " << simpleBaseTex.IsUseable() << ", " << shaderDiffuse.IsUseable() << std::endl;

        auto simpleEffect = effectMgr.CreateEffect("BaseShaderTexture.fxc");
        auto& simpleBaseTex = simpleEffect->GetParameter("baseTex");

        auto simpleTest = effectMgr.CreateEffect("test.fxc");
        auto& testTex = simpleTest->GetParameter("diffuse");

        Font font(renderer, (Common::ConfDir::Client() / "fonts" / "DejaVuSerif-Italic.ttf").string(), 32);

        Utils::Image img(renderer);

        glm::mat4 projection = glm::perspective(90.0f, (float)window.GetSize().x / (float)window.GetSize().y, 0.01f, 5.0f);
        glm::mat4 view = glm::lookAt(glm::vec3(0.001f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0, 1, 0));
        Tools::Frustum camera(glm::highp_mat4(projection * view));

        renderer.SetClearColor(glm::vec4(0, 0, 0.5f, 1));
        renderer.SetCullMode(CullMode::Clockwise);

        auto renderTarget = renderer.CreateRenderTarget(glm::uvec2(1024));
        renderTarget->PushRenderTarget(PixelFormat::Rgba8, RenderTargetUsage::Color);

        auto renderTarget2 = renderer.CreateRenderTarget(glm::uvec2(1024));
        renderTarget2->PushRenderTarget(PixelFormat::Rgba8, RenderTargetUsage::Color);
        //renderTarget2->PushRenderTarget(PixelFormat::Depth24Stencil8, RenderTargetUsage::DepthStencil);

        auto frontRt = renderTarget.get();
        auto backRt = renderTarget2.get();


        auto& test = simpleTest->GetParameter("test");


        int i = 0;
        while (run)
        {
            window.GetInputManager().ShowMouse();
            window.GetInputManager().ProcessEvents();
            window.GetInputManager().Dispatch(window.GetInputManager(), true);

            if (reload)
            {
                reload = false;
                Tools::log << "Reloading shaders...\n";
            }

            std::swap(frontRt, backRt);


            renderer.BeginDraw(frontRt);
            {
                renderer.BeginDraw2D();
                {
                    renderer.SetClearColor(glm::vec4(0, 0, 0.5f, 1));
                    renderer.Clear(ClearFlags::Color/* | ClearFlags::Depth | ClearFlags::Stencil*/);

                    do
                    {
                        simpleEffect->BeginPass();

                        renderer.SetModelMatrix(glm::scale(glm::vec3(frontRt->GetSize().x, frontRt->GetSize().y, 1)) * glm::translate(glm::vec3(0.5f, 0.5f, 0)));
                        img.Render(simpleBaseTex, *texture);

                        renderer.SetModelMatrix(glm::translate(glm::vec3(10, 10, -1)) * glm::scale(glm::vec3(1.0f)));
                        font.Render(simpleBaseTex, Tools::ToString(i++));
                    } while (simpleEffect->EndPass());
                }
                renderer.EndDraw2D();

                renderer.SetDepthTest(true);
                renderer.SetDepthWrite(true);
                renderer.SetProjectionMatrix(projection);
                renderer.SetViewMatrix(view);
                renderer.SetCullMode(CullMode::Clockwise);

                texture->Bind();
                backRt->GetTexture(0).Bind();
                do
                {
                    simpleTest->BeginPass();

                    test.Set(glm::vec4(2.0f, 1.0f, 1.0f, 1.0f));
                    testTex.Set(backRt->GetTexture(0));
                    renderer.SetModelMatrix(
                        glm::translate(glm::vec3(1.5f, 0.0050f, 0.0f))
                        * glm::yawPitchRoll(
                            timer.GetElapsedTime() * 0.001f,
                            timer.GetElapsedTime() * 0.0001f,
                            timer.GetElapsedTime() * 0.002f));
                    cube.Render();

                    //testTex.Set(*texture);
                    //renderer.SetModelMatrix(
                    //    glm::translate(glm::vec3(3.0f, -0.50f, 0.0f))
                    //    * glm::yawPitchRoll(
                    //        timer.GetElapsedTime() * 0.0001f,
                    //        timer.GetElapsedTime() * 0.00001f,
                    //        timer.GetElapsedTime() * 0.0002f));
                    //cube.Render();

                    //font.Render(testTex, "coucou", true);
                } while (simpleTest->EndPass());
                backRt->GetTexture(0).Unbind();
                texture->Unbind();
            }
            renderer.EndDraw();

            renderer.BeginDraw2D();
            {
                renderer.SetClearColor(glm::vec4(0.1f, 0.1f, 0.1f, 1));
                renderer.Clear(ClearFlags::Color | ClearFlags::Depth | ClearFlags::Stencil);
                renderer.SetModelMatrix(
                    glm::translate(glm::vec3(window.GetSize().x / 2, window.GetSize().y / 2, 0))
                    * glm::scale(glm::vec3(1024, 1024, 1))
                );

                do
                {
                    simpleEffect->BeginPass();
                    img.Render(simpleBaseTex, frontRt->GetTexture(0));

                } while (simpleEffect->EndPass());
            }
            renderer.EndDraw2D();

            window.Render();
        }
    }

    //std::cin.get();
    return 0;
}
