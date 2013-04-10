#include "tools/precompiled.hpp"

#include "tools/ByteArray.hpp"
#include "tools/gfx/effect/RenderState.hpp"
#include "tools/gfx/effect/Shader.hpp"

namespace Tools { namespace Gfx { namespace Effect {

    namespace {
        bool _ParseBool(std::string const& value)
        {
            return value == "true" || (value != "false" && value != "0");
        }

        float _ParseFloat(std::string const& value)
        {
            try
            {
                return boost::lexical_cast<float>(value);
            }
            catch (boost::bad_lexical_cast&)
            {
                return 0.0f;
            }
        }

        CullMode::Type _ParseCullMode(std::string const& value)
        {
            if (value == "none")
                return CullMode::None;
            if (value == "clockwise")
                return CullMode::Clockwise;
            if (value == "counterclockwise")
                return CullMode::CounterClockwise;
            return CullMode::None;
        }

        AlphaFunc::Type _ParseAlphaFunc(std::string const& value)
        {
            if (value == "never")
                return AlphaFunc::Never;
            if (value == "less")
                return AlphaFunc::Less;
            if (value == "lessequal")
                return AlphaFunc::LessEqual;
            if (value == "equal")
                return AlphaFunc::Equal;
            if (value == "greater")
                return AlphaFunc::Greater;
            if (value == "notequal")
                return AlphaFunc::NotEqual;
            if (value == "greaterequal")
                return AlphaFunc::GreaterEqual;
            if (value == "always")
                return AlphaFunc::Always;
            return AlphaFunc::Always;
        }

        Blend::Type _ParseBlend(std::string const& value)
        {
            if (value == "zero")
                return Blend::Zero;
            if (value == "one")
                return Blend::One;
            if (value == "srccolor")
                return Blend::SrcColor;
            if (value == "invsrccolor")
                return Blend::InvSrcColor;
            if (value == "srcalpha")
                return Blend::SrcAlpha;
            if (value == "invsrcalpha")
                return Blend::InvSrcAlpha;
            if (value == "destalpha")
                return Blend::DestAlpha;
            if (value == "invdestalpha")
                return Blend::InvDestAlpha;
            if (value == "destcolor")
                return Blend::DestColor;
            if (value == "invdestcolor")
                return Blend::InvDestColor;
            if (value == "srcalphasat")
                return Blend::SrcAlphaSat;
            if (value == "srccolor2")
                return Blend::SrcColor2;
            if (value == "invsrccolor2")
                return Blend::InvSrcColor2;
            return Blend::Zero;
        }

        BlendOp::Type _ParseBlendOp(std::string const& value)
        {
            if (value == "add")
                return BlendOp::Add;
            if (value == "subtract")
                return BlendOp::Subtract;
            if (value == "revsubtract")
                return BlendOp::RevSubtract;
            if (value == "min")
                return BlendOp::Min;
            if (value == "max")
                return BlendOp::Max;
            return BlendOp::Add;
        }

        FillMode::Type _ParseFillMode(std::string const& value)
        {
            if (value == "point")
                return FillMode::Point;
            if (value == "line")
                return FillMode::Line;
            if (value == "fill")
                return FillMode::Fill;
            return FillMode::Fill;
        }

        ZFunc::Type _ParseZFunc(std::string const& value)
        {
            if (value == "never")
                return ZFunc::Never;
            if (value == "less")
                return ZFunc::Less;
            if (value == "lessequal")
                return ZFunc::LessEqual;
            if (value == "equal")
                return ZFunc::Equal;
            if (value == "greater")
                return ZFunc::Greater;
            if (value == "notequal")
                return ZFunc::NotEqual;
            if (value == "greaterequal")
                return ZFunc::GreaterEqual;
            if (value == "always")
                return ZFunc::Always;
            return ZFunc::Always;
        }
    }

    RenderState::RenderState() :
        cullMode(CullMode::None),
        alphaBlendEnable(true),
        alphaFunc(AlphaFunc::Always),
        alphaRef(0.0f),
        alphaTestEnable(false),
        srcBlend(Blend::One),
        destBlend(Blend::Zero),
        blendOp(BlendOp::Add),
        srcBlendAlpha(Blend::One),
        destBlendAlpha(Blend::Zero),
        blendOpAlpha(BlendOp::Add),
        ditherEnable(false),
        fillMode(FillMode::Fill),
        zEnable(false),
        zWriteEnable(true),
        zFunc(ZFunc::Less)
    {
    }

    void RenderState::Load(Shader& shader)
    {
        std::map<std::string, std::function<void(std::string const&)>> tokens;
        tokens["cullmode"] = [&](std::string const& value) { this->cullMode = _ParseCullMode(value); };
        tokens["alphablendenable"] = [&](std::string const& value) { this->alphaBlendEnable = _ParseBool(value); };
        tokens["alphafunc"] = [&](std::string const& value) { this->alphaFunc = _ParseAlphaFunc(value); };
        tokens["alpharef"] = [&](std::string const& value) { this->alphaRef = _ParseFloat(value); };
        tokens["alphatestenable"] = [&](std::string const& value) { this->alphaTestEnable = _ParseBool(value); };
        tokens["srcblend"] = [&](std::string const& value) { this->srcBlend = _ParseBlend(value); };
        tokens["destblend"] = [&](std::string const& value) { this->destBlend = _ParseBlend(value); };
        tokens["blendop"] = [&](std::string const& value) { this->blendOp = _ParseBlendOp(value); };
        tokens["srcblendalpha"] = [&](std::string const& value) { this->srcBlendAlpha = _ParseBlend(value); };
        tokens["destblendalpha"] = [&](std::string const& value) { this->destBlendAlpha = _ParseBlend(value); };
        tokens["blendopalpha"] = [&](std::string const& value) { this->blendOpAlpha = _ParseBlendOp(value); };
        tokens["ditherenable"] = [&](std::string const& value) { this->ditherEnable = _ParseBool(value); };
        tokens["fillmode"] = [&](std::string const& value) { this->fillMode = _ParseFillMode(value); };
        tokens["zenable"] = [&](std::string const& value) { this->zEnable = _ParseBool(value); };
        tokens["zfunc"] = [&](std::string const& value) { this->zFunc = _ParseZFunc(value); };
        tokens["zwriteenable"] = [&](std::string const& value) { this->zWriteEnable = _ParseBool(value); };

        for (auto const& pair: shader.deviceStates)
        {
            std::string key = boost::algorithm::to_lower_copy(pair.first);
            std::string value = boost::algorithm::to_lower_copy(pair.second);
            auto& it = tokens.find(key);
            if (it != tokens.end())
                it->second(value);
            else
                Tools::error << "Unknown render state \"" << key << "\"\n";
        }
    }

    void RenderState::Set(IRenderer& renderer)
    {
        // TODO optim
        renderer.SetCullMode(this->cullMode);
        renderer.SetAlphaBlendEnable(this->alphaBlendEnable);
        renderer.SetAlphaFunc(this->alphaFunc);
        renderer.SetAlphaRef(this->alphaRef);
        renderer.SetAlphaTestEnable(this->alphaTestEnable);
        renderer.SetSrcBlend(this->srcBlend);
        renderer.SetDestBlend(this->destBlend);
        renderer.SetBlendOp(this->blendOp);
        renderer.SetSrcBlendAlpha(this->srcBlendAlpha);
        renderer.SetDestBlendAlpha(this->destBlendAlpha);
        renderer.SetBlendOpAlpha(this->blendOpAlpha);
        renderer.SetDitherEnable(this->ditherEnable);

        renderer.SetFillMode(this->fillMode);
        renderer.SetZEnable(this->zEnable);
        renderer.SetZFunc(this->zFunc);
        renderer.SetZWriteEnable(this->zWriteEnable);
    }
}}}
