#include "tools/precompiled.hpp"
#include "resources/resources.hpp"
#include "tools/renderers/utils/Image.hpp"

namespace Tools { namespace Renderers { namespace Utils {

    Image::Image(IRenderer& renderer, std::unique_ptr<Renderers::ITexture2D> texture)
        : _renderer(renderer),
        _texture(texture.release())
    {
        Image::_InitShader(renderer);
    }

    Image::Image(IRenderer& renderer, std::string const& imagePath)
        : _renderer(renderer),
        _texture(renderer.CreateTexture2D(imagePath).release())
    {
        Image::_InitShader(renderer);
    }

    Image::~Image()
    {
        Delete(this->_texture);
    }

    void Image::Render()
    {
        unsigned char indices[] = { 2, 1, 3, 0 };

        Image::_vertexBuffer->Bind();
        this->_texture->Bind();
        Image::_textureParameter->Set(*this->_texture);

        do
        {
            Image::_shader->BeginPass();
            this->_renderer.DrawElements(sizeof(indices), Renderers::DataType::UnsignedByte, indices, Renderers::DrawingMode::TrianglesStrip);
        } while (Image::_shader->EndPass());

        this->_texture->Unbind();
        Image::_vertexBuffer->Unbind();
    }

    IVertexBuffer* Image::_vertexBuffer = 0;
    IShaderProgram* Image::_shader = 0;
    IShaderParameter* Image::_textureParameter = 0;

    void Image::_InitShader(IRenderer& renderer)
    {
        if (Image::_shader != 0)
            return;
        // TODO: trouver un moyen de gérer ce type de resources (utile avant une connexion à une partie donc pas de ResourceManager)
        // TODO: Faire une seconde classe ou gérer le shader dans la font pour la possibilité d'avoir un shader différent par font
        std::ifstream t(RESOURCES_DIR "/BaseShaderTexture.cgfx");
        std::string shader((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());

        Image::_shader = renderer.CreateProgram(shader).release();
        Image::_textureParameter = Image::_shader->GetParameter("baseTex").release();

        const float vertices[] =
        {
            -1,  1, 0,  0, 0,
             1,  1, 0,  1, 0,
             1, -1, 0,  1, 1,
            -1, -1, 0,  0, 1,
        };
        Image::_vertexBuffer = renderer.CreateVertexBuffer().release();
        Image::_vertexBuffer->PushVertexAttribute(Renderers::DataType::Float, Renderers::VertexAttributeUsage::Position, 3); // position
        Image::_vertexBuffer->PushVertexAttribute(Renderers::DataType::Float, Renderers::VertexAttributeUsage::TexCoord, 2); // texCoord
        Image::_vertexBuffer->SetData(4*3*sizeof(float) + 4*2*sizeof(float), vertices, Renderers::VertexBufferUsage::Static);
    }

}}}
