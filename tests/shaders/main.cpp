#include "tests/shaders/precompiled.hpp"

#include "tools/IRenderer.hpp"
#include "tools/renderers/utils/GBuffer.hpp"
#include "tools/window/InputManager.hpp"
#include "tools/window/sdl/Window.hpp"

using namespace Tools;
using namespace Tools::Renderers;
using namespace Tools::Window;

#undef main

static std::string testShaderPath = "test.fx";
static std::string texturePath = "test.png";

static std::unique_ptr<IShaderProgram> testShader;
static std::unique_ptr<IShaderParameter> colors;
static std::unique_ptr<IShaderParameter> normals;
static std::unique_ptr<IShaderParameter> depth;

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
        0, 1, 2,  0, 2, 3
    };
    indexBuffer.SetData(DataType::UnsignedShort, sizeof(indices), indices);
}

static void LoadShaders(IRenderer& renderer)
{
    std::ifstream file(testShaderPath);
    testShader = renderer.CreateProgram(std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>()));
    colors = testShader->GetParameter("colors");
    normals = testShader->GetParameter("normals");
    depth = testShader->GetParameter("depthBuffer");
}

static void UnloadShaders()
{
    colors = 0;
    normals = 0;
    depth = 0;
    testShader = 0;
}

static void Render(IRenderer& renderer, Utils::GBuffer& gbuffer, ITexture2D& texture)
{
    texture.Bind();

    colors->Set(texture);
    //normals->Set();
    //depth->Set();

    do
    {
        testShader->BeginPass();
        renderer.DrawElements(1*3*2, DataType::UnsignedShort);
    } while (testShader->EndPass());

    texture.Unbind();
}

int main(int ac, char *av[])
{
    if (ac > 1)
        testShaderPath = av[1];
    if (ac > 2)
        texturePath = av[2];

    std::map<std::string, Tools::Window::BindAction::BindAction> actions;
    actions["quit"] = BindAction::Quit;
    actions["reloadshaders"] = BindAction::ReloadShaders;

    Tools::Window::Window& window = *new Sdl::Window(actions, true);
    IRenderer& renderer = window.GetRenderer();
    bool run = true;
    bool reload = true;

    window.GetInputManager().Bind(BindAction::ReloadShaders, BindAction::Released, [&]() { reload = true; Tools::log << "r\n"; });
    window.GetInputManager().Bind(BindAction::Quit, BindAction::Released, [&]() { run = false; Tools::log << "Quit\n"; });

    window.GetInputManager().GetInputBinder().Bind("r", "reloadshaders");
    window.GetInputManager().GetInputBinder().Bind("escape", "quit");

    {
        auto texture = renderer.CreateTexture2D(texturePath);
        auto vertexBuffer = renderer.CreateVertexBuffer();
        auto indexBuffer = renderer.CreateIndexBuffer();
        CreateCube(*vertexBuffer, *indexBuffer);

        Utils::GBuffer gbuffer(renderer, window.GetSize());
        while (run)
        {
            window.GetInputManager().ShowMouse();
            window.GetInputManager().ProcessEvents();
            window.GetInputManager().Dispatch(window.GetInputManager(), true);

            if (reload)
            {
                UnloadShaders();
                LoadShaders(renderer);
                reload = false;
            }

            renderer.BeginDraw();
            gbuffer.Bind();
            gbuffer.Unbind();

            renderer.Clear(ClearFlags::Color | ClearFlags::Depth);

            renderer.SetProjectionMatrix(glm::perspective(90.0f, (float)window.GetSize().x / (float)window.GetSize().y, 0.01f, 50.0f));
            renderer.SetViewMatrix(glm::lookAt(glm::vec3(0.0f), glm::vec3(5.0f), glm::vec3(0, 1, 0)));
            renderer.SetModelMatrix(glm::translate(glm::vec3(1.0f) * 2.0f));

            //renderer.SetCullFace(false);
            vertexBuffer->Bind();
            indexBuffer->Bind();
            Render(renderer, gbuffer, *texture);
            indexBuffer->Unbind();
            vertexBuffer->Unbind();
            renderer.EndDraw();

            window.Render();
        }

        UnloadShaders();
    }

    delete &window;
    return 0;
}
