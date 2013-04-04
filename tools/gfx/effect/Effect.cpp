#include "tools/precompiled.hpp"

#include "tools/gfx/effect/Effect.hpp"
#include "tools/gfx/effect/Shader.hpp"

namespace Tools { namespace Gfx { namespace Effect {

    namespace {

        VertexAttributeUsage::Type GetAttributeUsage(Semantic::Type semantic)
        {
            switch (semantic)
            {
            case Semantic::Position: return VertexAttributeUsage::Position;
            case Semantic::Normal: return VertexAttributeUsage::Normal;
            case Semantic::Color: return VertexAttributeUsage::Color;
            case Semantic::TexCoord0: return VertexAttributeUsage::TexCoord0;
            case Semantic::TexCoord1: return VertexAttributeUsage::TexCoord1;
            case Semantic::TexCoord2: return VertexAttributeUsage::TexCoord2;
            case Semantic::TexCoord3: return VertexAttributeUsage::TexCoord3;
            case Semantic::TexCoord4: return VertexAttributeUsage::TexCoord4;
            default:
                throw std::invalid_argument("it's not an attribute");
            }
        }

        ShaderParameterUsage::Type GetShaderParameterUsage(Semantic::Type semantic)
        {
            switch (semantic)
            {
            case Semantic::Projection: return ShaderParameterUsage::ProjectionMatrix;
            case Semantic::View: return ShaderParameterUsage::ViewMatrix;
            case Semantic::World: return ShaderParameterUsage::ModelMatrix;

            case Semantic::WorldView: return ShaderParameterUsage::ModelViewMatrix;
            case Semantic::WorldViewProjection: return ShaderParameterUsage::ModelViewProjectionMatrix;
            case Semantic::ViewProjection: return ShaderParameterUsage::ViewProjectionMatrix;

            case Semantic::WorldInverse: return ShaderParameterUsage::WorldInverse;
            case Semantic::ViewInverse: return ShaderParameterUsage::ViewInverse;
            case Semantic::ProjectionInverse: return ShaderParameterUsage::ProjectionInverse;
            case Semantic::WorldViewInverse: return ShaderParameterUsage::WorldViewInverse;
            case Semantic::ViewProjectionInverse: return ShaderParameterUsage::ViewProjectionInverse;
            case Semantic::WorldViewProjectionInverse: return ShaderParameterUsage::WorldViewProjectionInverse;

            case Semantic::WorldInverseTranspose: return ShaderParameterUsage::WorldInverseTranspose;
            case Semantic::ViewInverseTranspose: return ShaderParameterUsage::ViewInverseTranspose;
            case Semantic::ProjectionInverseTranspose: return ShaderParameterUsage::ProjectionInverseTranspose;
            case Semantic::WorldViewInverseTranspose: return ShaderParameterUsage::WorldViewInverseTranspose;
            case Semantic::ViewProjectionInverseTranspose: return ShaderParameterUsage::ViewProjectionInverseTranspose;
            case Semantic::WorldViewProjectionInverseTranspose: return ShaderParameterUsage::WorldViewProjectionInverseTranspose;

            default:
                return ShaderParameterUsage::None;
            }
        }

        std::unique_ptr<ISamplerState> _CreateSamplerState(IRenderer& renderer, UniformParameter const& param)
        {
            auto sampler = renderer.CreateSamplerState();
            if (param.value.which() == 0)
            {
                auto const& states = boost::get<Sampler>(param.value).states;
                for (auto const& pair: states)
                {
                    std::cout << pair.first << " => " << pair.second << std::endl;
                }
            }
            else
            {
                auto const& str = boost::get<std::string>(param.value);
                int pos = 1;
                while (str[pos] != '}')
                {
                    auto readIdentifier = [&]() {
                        int startPos = pos;
                        while (str[pos] != '=' && str[pos] != ';')
                            ++pos;
                        return str.substr(startPos, pos - startPos);
                    };
                    auto parseValue = [](std::string const& value) {
                        if (value == "nearest")
                            return TextureFilter::Nearest;
                        if (value == "linear")
                            return TextureFilter::Linear;
                        return TextureFilter::Linear;
                    };

                    std::string key = readIdentifier();
                    pos++;
                    std::string value = readIdentifier();
                    pos++;

                    if (boost::algorithm::to_lower_copy(key) == "minfilter")
                        sampler->SetMinFilter(parseValue(boost::algorithm::to_lower_copy(value)));
                    if (boost::algorithm::to_lower_copy(key) == "magfilter")
                        sampler->SetMagFilter(parseValue(boost::algorithm::to_lower_copy(value)));

                    std::cout << key << " => " << value << std::endl;
                }
            }
            return sampler;
        }
    }

    TextureShaderParameter::TextureShaderParameter(IShaderParameter& param, std::unique_ptr<ISamplerState>&& sampler) :
        _parameter(param),
        _sampler(std::move(sampler))
    {
    }

    void TextureShaderParameter::Set(ITexture2D& tex)
    {
        this->_parameter.Set(tex, *this->_sampler);
    }

    void TextureShaderParameter::Set(ITexture2D& tex, ISamplerState& sampler)
    {
        this->_parameter.Set(tex, sampler);
    }

    Effect::Effect(IRenderer& renderer, CompleteShader const& complShader) :
        _renderer(renderer)
    {
        this->_shader = renderer.GetRendererName().find("DirectX") != -1 ? complShader.directX : complShader.openGL;
        this->_program = renderer.CreateProgram(this->_shader.vertex, this->_shader.pixel);
        for(auto const& pair: this->_shader.attributes)
            this->_program->SetAttributeUsage(pair.second.name, GetAttributeUsage(pair.second.semantic));
        for (auto const& pair: this->_shader.uniforms)
        {
            auto usage = GetShaderParameterUsage(pair.second.semantic);
            if (usage != ShaderParameterUsage::None)
                this->_program->SetParameterUsage(usage, pair.second.name);
        }
    }

    IShaderParameter& Effect::GetParameter(std::string const& identifier)
    {
        auto const& it = this->_shader.uniforms.find(identifier);
        if (it == this->_shader.uniforms.end())
        {
            auto& dummy = this->_dummyParameters[identifier];
            if (dummy.get() == nullptr)
                dummy.reset(new DummyShaderParameter());
            return *dummy;
        }
        if (it->second.type == Type::Sampler2D)
        {
            auto& texIt = this->_textureParameters.find(identifier);
            if (texIt == this->_textureParameters.end())
            {
                auto tsp = new TextureShaderParameter(this->_program->GetParameter(it->second.name), _CreateSamplerState(this->_renderer, it->second));
                this->_textureParameters[identifier].reset(tsp);
                return *tsp;
            }
            return *texIt->second;
        }
        return this->_program->GetParameter(it->second.name);
    }

}}}
