#include "tests/shaders/precompiled.hpp"

#include "tools/renderers/utils/GBuffer.hpp"
#include "tools/renderers/utils/Image.hpp"
#include "tools/renderers/utils/Rectangle.hpp"
#include "tools/window/InputManager.hpp"
#include "tools/window/sdl/Window.hpp"
#include "tools/IRenderer.hpp"
#include "tools/Timer.hpp"
#include "tools/Vector2.hpp"
#include "tools/Vector3.hpp"

using namespace Tools;
using namespace Tools::Renderers;
using namespace Tools::Window;

#undef main

static std::string testShaderPath = "test.fx";
static std::string combineShaderPath = "combine.fx";
static std::string texturePath = "test.png";

static std::string codeTestShader;
static std::string codeCombineShader;

static std::unique_ptr<Utils::GBuffer> gbuffer;
static std::unique_ptr<IShaderProgram> testShader;
static std::unique_ptr<IShaderParameter> diffuse;
static std::unique_ptr<IShaderProgram> combineShader;
static std::unique_ptr<IShaderParameter> colors;
static std::unique_ptr<IShaderParameter> normals;
static std::unique_ptr<IShaderParameter> depth;
static std::unique_ptr<IShaderParameter> quadWorldViewProjection;

static std::unique_ptr<ITexture2D> texture;
static std::unique_ptr<IVertexBuffer> vertexBuffer;
static std::unique_ptr<IIndexBuffer> indexBuffer;
static std::unique_ptr<Utils::Image> screenQuad;

struct RAII
{
    std::function<void(void)> destroy;
    RAII(std::function<void(void)> destroy) : destroy(destroy) {}
    ~RAII() { destroy(); }
};

static void CreateCube(IVertexBuffer& vertexBuffer, IIndexBuffer& indexBuffer)
{
    struct Vertex
    {
        glm::fvec3 position;
        glm::fvec3 normal;
        glm::fvec2 texture;
        Vertex() {}
        Vertex(glm::fvec3 const& position, glm::fvec3 const& normal, glm::fvec2 const& texture) : position(position), normal(normal), texture(texture) {}
    };

    static glm::fvec3 const positions[] = {
        glm::fvec3(0, 1, 1), // frontTopLeft = 0;
        glm::fvec3(1, 1, 1), // frontTopRight = 1;
        glm::fvec3(1, 1, 0), // backTopRight = 2;
        glm::fvec3(0, 1, 0), // backTopLeft = 3;
        glm::fvec3(0, 0, 0), // backBottomLeft = 4;
        glm::fvec3(1, 0, 0), // backBottomRight = 5;
        glm::fvec3(1, 0, 1), // frontBottomRight = 6;
        glm::fvec3(0, 0, 1), // frontBottomLeft = 7;
    };
    static glm::fvec3 const normals[] = {
        glm::fvec3(0, 0, 1), // front = 0;
        glm::fvec3(0, 1, 0), // top = 1;
        glm::fvec3(1, 0, 0), // right = 2;
        glm::fvec3(0, -1, 0), // bottom = 3;
        glm::fvec3(-1, 0, 0), // left = 4;
        glm::fvec3(0, 0, -1), // back = 5;
    };
    glm::fvec2 const textures[] = {
        glm::fvec2(1, 0),
        glm::fvec2(0, 0),
        glm::fvec2(0, 1),
        glm::fvec2(1, 1),
    };
    static unsigned short positionIndices[][4] = {
        {6, 1, 0, 7}, // front = 0;
        {0, 1, 2, 3}, // top = 1;
        {5, 2, 1, 6}, // right = 2;
        {4, 5, 6, 7}, // bottom = 3;
        {7, 0, 3, 4}, // left = 4;
        {4, 3, 2, 5}, // back = 5;
    };

    std::vector<Vertex> vertices(6*4); // nb faces * nb vertices
    size_t offset = 0;
    for (int idx = 0; idx < 6; ++idx)
    {
        unsigned short *pos = positionIndices[idx];
        vertices[offset++] = Vertex(positions[*(pos++)] - 0.5f, normals[idx], textures[0]);
        vertices[offset++] = Vertex(positions[*(pos++)] - 0.5f, normals[idx], textures[3]);
        vertices[offset++] = Vertex(positions[*(pos++)] - 0.5f, normals[idx], textures[2]);
        vertices[offset++] = Vertex(positions[*(pos++)] - 0.5f, normals[idx], textures[1]);
    }

    vertexBuffer.PushVertexAttribute(DataType::Float, VertexAttributeUsage::Position, 3);
    vertexBuffer.PushVertexAttribute(DataType::Float, VertexAttributeUsage::Normal, 3);
    vertexBuffer.PushVertexAttribute(DataType::Float, VertexAttributeUsage::TexCoord, 2);
    vertexBuffer.SetData(offset * sizeof(Vertex), vertices.data(), VertexBufferUsage::Static);

    static unsigned short indices[] = {
         0,  1,  2,   0,  2,  3,
         4,  5,  6,   4,  6,  7,
         8,  9, 10,   8, 10, 11,
        12, 13, 14,  12, 14, 15,
        16, 17, 18,  16, 18, 19,
        20, 21, 22,  20, 22, 23
    };
    indexBuffer.SetData(DataType::UnsignedShort, sizeof(indices), indices);
}

static void LoadShaders(Sdl::Window& window)
{
    IRenderer& renderer = window.GetRenderer();
    try
    {
        {
            std::ifstream file(testShaderPath);
            auto tmp = std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            testShader = renderer.CreateProgram(tmp);
            codeTestShader = std::move(tmp);
        }
        {
            std::ifstream file(combineShaderPath);
            auto tmp = std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            combineShader = renderer.CreateProgram(tmp);
            codeCombineShader = std::move(tmp);
        }
    }
    catch (std::exception& ex)
    {
        Tools::error << ex.what() << std::endl;
        testShader = renderer.CreateProgram(codeTestShader);
        combineShader = renderer.CreateProgram(codeCombineShader);
    }

    diffuse = testShader->GetParameter("diffuse");
    colors = combineShader->GetParameter("colors");
    normals = combineShader->GetParameter("normalsDepth");
    //depth = combineShader->GetParameter("depthBuffer");
    quadWorldViewProjection = combineShader->GetParameter("quadWorldViewProjection");

    gbuffer = std::unique_ptr<Utils::GBuffer>(new Utils::GBuffer(renderer, window.GetSize(), *combineShader));
}

static void UnloadShaders()
{
    gbuffer = 0;
    quadWorldViewProjection = 0;
    diffuse = 0;
    colors = 0;
    normals = 0;
    depth = 0;
    combineShader = 0;
    testShader = 0;
}

static void RenderCube(IRenderer& renderer, ITexture2D& cubeTexture)
{
    vertexBuffer->Bind();
    indexBuffer->Bind();
    cubeTexture.Bind();

    diffuse->Set(cubeTexture);

    do
    {
        testShader->BeginPass();
        renderer.DrawElements(6*3*2, DataType::UnsignedShort);
    } while (testShader->EndPass());

    cubeTexture.Unbind();
    indexBuffer->Unbind();
    vertexBuffer->Unbind();
}

static void RenderGBuffer(IRenderer& renderer)
{
    //gbuffer->GetNormalsDepth().Bind();
    //normals->Set(gbuffer->GetNormalsDepth());
    //do
    //{
    //    combineShader->BeginPass();
    //    screenQuad->Render(*colors, gbuffer->GetColors());
    //} while (combineShader->EndPass());
    //gbuffer->GetNormalsDepth().Unbind();
}

int main(int ac, char *av[])
{
    if (ac > 1)
        testShaderPath = av[1];
    if (ac > 2)
        combineShaderPath = av[2];
    if (ac > 3)
        texturePath = av[3];

    Tools::log << "Current directory " << boost::filesystem::current_path() << std::endl;
    Tools::log << "Loading \"" << testShaderPath + "\", \"" << combineShaderPath << "\", \"" << texturePath << "\"\n";

    std::map<std::string, Tools::Window::BindAction::BindAction> actions;
    actions["quit"] = BindAction::Quit;
    actions["reloadshaders"] = BindAction::ReloadShaders;
    actions["togglecullface"] = BindAction::ToggleCullface;

    Sdl::Window window(actions, true);
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

    {
        Timer timer;

        texture = renderer.CreateTexture2D(texturePath);
        vertexBuffer = renderer.CreateVertexBuffer();
        indexBuffer = renderer.CreateIndexBuffer();
        screenQuad = std::unique_ptr<Utils::Image>(new Utils::Image(renderer));
        RAII _shaders([&]() { UnloadShaders(); texture = 0; vertexBuffer = 0; indexBuffer = 0; screenQuad = 0; });

        CreateCube(*vertexBuffer, *indexBuffer);

        while (run)
        {
            window.GetInputManager().ShowMouse();
            window.GetInputManager().ProcessEvents();
            window.GetInputManager().Dispatch(window.GetInputManager(), true);

            if (reload)
            {
                UnloadShaders();
                LoadShaders(window);
                reload = false;
                Tools::log << "Reloading shaders...\n";
            }

            renderer.SetProjectionMatrix(glm::perspective(90.0f, (float)window.GetSize().x / (float)window.GetSize().y, 0.01f, 5.0f));
            renderer.SetViewMatrix(glm::lookAt(glm::vec3(0.001f), glm::vec3(1.0f, 0.0f, 1.0f), glm::vec3(0, 1, 0)));
            renderer.SetModelMatrix(
                glm::translate(glm::vec3(1.0f, 0.0f, 1.0f))
                * glm::yawPitchRoll(
                    timer.GetElapsedTime() * 0.001f,
                    timer.GetElapsedTime() * 0.0001f,
                    timer.GetElapsedTime() * 0.002f));

            renderer.BeginDraw();

            gbuffer->Bind();
            renderer.SetClearColor(Color4f(0.0f, 0.0f, 0.0f, 0.0f));
            renderer.Clear(ClearFlags::Color | ClearFlags::Depth);

            renderer.SetCullFace(cullface);

            RenderCube(renderer, *texture);
            gbuffer->Unbind();

            renderer.SetClearColor(Color4f(0.0f, 0.0f, 0.5f, 1.0f));
            renderer.Clear(ClearFlags::Color | ClearFlags::Depth);
            RenderCube(renderer, gbuffer->GetColors());

            gbuffer->Render();

            renderer.EndDraw();

            window.Render();
        }
    }

    return 0;
}
