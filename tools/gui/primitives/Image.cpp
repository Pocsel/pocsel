
#include "Image.hpp"

namespace Tools { namespace Gui { namespace Primitives {
    Renderers::IVertexBuffer* Image::_vertexBuffer = 0;
    Renderers::IShaderProgram* Image::_shader = 0;
    Renderers::IShaderParameter* Image::_textureParameter = 0;

    void (*Image::_renderFunc)(Image&, IRenderer&) = &Image::_InitRender;

    void Image::_InitRender(Image& img, IRenderer& renderer)
    {
        char const* vShader =
            "uniform mat4 u_mvp;"
            ""
            "void main() {"
            "   gl_TexCoord[0] = gl_MultiTexCoord0;"
            "   gl_Position = u_mvp * gl_Vertex;"
            "}";
        char const* fShader =
            "uniform sampler2D u_tex;"
            ""
            "void main() {"
            "   gl_FragColor = texture2D(u_tex, gl_TexCoord[0].st);"
            "}";

        Image::_shader = renderer.CreateProgram(vShader, fShader).release();
        Image::_shader->SetParameterUsage("u_mvp", Renderers::ShaderParameterUsage::ModelViewProjectionMatrix);
        Image::_textureParameter = Image::_shader->GetParameter("u_tex").release();

        const float vertices[] =
        {
            -1, 1, 0,
            1, 1, 0,
            1, -1, 0,
            -1, -1, 0
        };

        Image::_vertexBuffer = renderer.CreateVertexBuffer().release();

        Image::_vertexBuffer->PushVertexAttribute(Renderers::DataType::Float, Renderers::VertexAttributeUsage::Position, 3); // position
        Image::_vertexBuffer->SetData(4*3*sizeof(float), vertices, Renderers::VertexBufferUsage::Static);

        Image::_renderFunc = &Image::_Render;
        Image::_Render(img, renderer);
    }

    void Image::_Render(Image& img, IRenderer& renderer)
    {
        if (img._uvBuffer == 0)
        {
            const float uvs[] = {
                0, 0,
                1, 0,
                1, 1,
                0, 1
            };

            img._uvBuffer = renderer.CreateVertexBuffer().release();
            img._uvBuffer->PushVertexAttribute(Renderers::DataType::Float, Renderers::VertexAttributeUsage::TexCoord, 2);
            img._uvBuffer->SetData(4*2*sizeof(float), uvs, Renderers::VertexBufferUsage::Dynamic);
        }
        unsigned char indices[] = { 2, 1, 3, 0 };

        Image::_vertexBuffer->Bind();
        img._uvBuffer->Bind();
        img._texture->Bind();

        Image::_shader->Activate();
        Image::_textureParameter->Set(*img._texture);

        renderer.DrawElements(sizeof(indices), Renderers::DataType::UnsignedByte, indices, Renderers::DrawingMode::TrianglesStrip);

        img._texture->Unbind();
        img._uvBuffer->Unbind();
        Image::_vertexBuffer->Unbind();
    }

    Image::Image(std::unique_ptr<Renderers::ITexture2D> texture)
    {
        this->_CreateFromTexture(std::move(texture));
    }

    Image::Image(IRenderer& r, std::string const& imagePath)
    {
        this->_CreateFromTexture(r.CreateTexture2D(imagePath));
    }

    Image::~Image()
    {
        Delete(this->_texture);
        Delete(this->_uvBuffer);
    }

    void Image::_CreateFromTexture(std::unique_ptr<Renderers::ITexture2D> texture)
    {
        this->_uvBuffer = 0;
        this->_texture = texture.release();
    }

}}}
