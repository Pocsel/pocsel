#include "tests/shaders/precompiled.hpp"

#include "tools/renderers/effect/Shader.hpp"
#include "tools/renderers/effect/ShaderSerializer.hpp"
#include "tools/renderers/utils/GBuffer.hpp"
#include "tools/renderers/utils/Image.hpp"
#include "tools/renderers/utils/Rectangle.hpp"
#include "tools/renderers/utils/Sphere.hpp"
#include "tools/window/InputManager.hpp"
#include "tools/window/sdl/Window.hpp"
#include "tools/ByteArray.hpp"
#include "tools/IRenderer.hpp"
#include "tools/Timer.hpp"
#include "tools/Vector2.hpp"
#include "tools/Vector3.hpp"

#undef main

using namespace Tools;
using namespace Tools::Renderers;
using namespace Tools::Window;

struct RAII
{
    std::function<void(void)> destroy;
    RAII(std::function<void(void)> destroy) : destroy(destroy) {}
    ~RAII() { destroy(); }
};

class Cube
{
private:
    IRenderer& _renderer;
    std::unique_ptr<Renderers::IIndexBuffer> _indexBuffer;
    std::unique_ptr<Renderers::IVertexBuffer> _vertexBuffer;

public:
    Cube(IRenderer& renderer) :
        _renderer(renderer),
        _indexBuffer(renderer.CreateIndexBuffer()),
        _vertexBuffer(renderer.CreateVertexBuffer())
    {
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

        struct Vertex
        {
            glm::fvec3 position;
            glm::fvec3 normal;
            glm::fvec2 texture;
            Vertex() {}
            Vertex(glm::fvec3 const& position, glm::fvec3 const& normal, glm::fvec2 const& texture) : position(position), normal(normal), texture(texture) {}
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

        _vertexBuffer->PushVertexAttribute(DataType::Float, VertexAttributeUsage::Position, 3);
        _vertexBuffer->PushVertexAttribute(DataType::Float, VertexAttributeUsage::Normal, 3);
        _vertexBuffer->PushVertexAttribute(DataType::Float, VertexAttributeUsage::TexCoord0, 2);
        _vertexBuffer->SetData(offset * sizeof(Vertex), vertices.data(), VertexBufferUsage::Static);

        static unsigned short indices[] = {
             0,  1,  2,   0,  2,  3,
             4,  5,  6,   4,  6,  7,
             8,  9, 10,   8, 10, 11,
            12, 13, 14,  12, 14, 15,
            16, 17, 18,  16, 18, 19,
            20, 21, 22,  20, 22, 23
        };
        _indexBuffer->SetData(DataType::UnsignedShort, sizeof(indices), indices);
    }

    void Render()
    {
        _indexBuffer->Bind();
        _vertexBuffer->Bind();
        _renderer.DrawElements(6 * 3 * 2, DataType::UnsignedShort);
        _vertexBuffer->Unbind();
        _indexBuffer->Unbind();
    }
};

namespace {

    ByteArray ReadFile(std::string const& filename)
    {
        std::ifstream in(filename, std::istream::binary | std::ios::in);
        std::istreambuf_iterator<char> it(in);
        std::vector<char> tmp;
        while (in)
        {
            char buf[8192];
            in.read(buf, sizeof(buf));
            auto readSize = in.gcount();
            auto oldSize = tmp.size();
            tmp.resize(oldSize + readSize);
            std::memcpy(tmp.data() + oldSize, buf, readSize);
        }
        return ByteArray(tmp.data(), tmp.size());
    }

}

int main(int ac, char *av[])
{
    //if (ac > 1)
    //    testShaderPath = av[1];
    //if (ac > 2)
    //    directionnalLightShaderPath = av[2];
    //if (ac > 3)
    //    combineShaderPath = av[3];
    //if (ac > 4)
    //    texturePath = av[4];

    //Tools::log << "Current directory " << boost::filesystem::current_path() << std::endl;
    //Tools::log << "Loading \"" << testShaderPath + "\", \"" << combineShaderPath << "\", \"" << texturePath << "\"\n";

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
    //window.RegisterCallback([](glm::uvec2 const& size) { if (gbuffer) gbuffer->Resize(size); });

    {
        auto shader = ReadFile("D:\\Programmation\\C++\\pocsel\\build_x64_vs11\\bin\\Debug\\test.fxc").Read<Effect::CompleteShader>();

        Timer timer;
        Cube cube(renderer);
        auto program = renderer.CreateProgram(shader->directX.source);

        while (run)
        {
            window.GetInputManager().ShowMouse();
            window.GetInputManager().ProcessEvents();
            window.GetInputManager().Dispatch(window.GetInputManager(), true);

            renderer.SetProjectionMatrix(glm::perspective(90.0f, (float)window.GetSize().x / (float)window.GetSize().y, 0.01f, 5.0f));
            renderer.SetViewMatrix(glm::lookAt(glm::vec3(0.001f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0, 1, 0)));

            renderer.BeginDraw();
            renderer.Clear(ClearFlags::Color | ClearFlags::Depth | ClearFlags::Stencil);

            do
            {
                program->BeginPass();
                renderer.SetModelMatrix(
                    glm::translate(glm::vec3(2.0f, 0.0f, 0.0f))
                    * glm::yawPitchRoll(
                        timer.GetElapsedTime() * 0.0001f,
                        timer.GetElapsedTime() * 0.00001f,
                        timer.GetElapsedTime() * 0.0002f));
                cube.Render();
            }
            while (!program->EndPass());

            renderer.EndDraw();

            window.Render();
        }
    }

    return 0;
}
