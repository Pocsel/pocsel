#pragma once

#include "tools/ByteArray.hpp"
#include "tools/renderers/effect/Shader.hpp"

namespace Tools {

    template<>
    struct ByteArray::Serializer<Renderers::Effect::Shader>
    {
        static void Read(ByteArray const& p, Renderers::Effect::Shader& shader)  // Used by Packet::Read<T>(T&)
        {
            using namespace Renderers::Effect;
            auto readLongString = [&]() { auto s = p.Read32(); return std::string(p.ReadRawData(s), s); };

            // magic code
            if (p.Read32() != 0xB16B00B5)
                throw std::runtime_error("Where is bigboobs ?!");

            // shaders
            shader.source = readLongString();
            shader.glslVertex = readLongString();
            shader.glslPixel = readLongString();
            shader.hlslVertex = readLongString();
            shader.hlslPixel = readLongString();

            // Attributes
            auto count = p.Read32();
            for (Uint32 i = 0; i < count; ++i)
            {
                BaseParameter param;
                auto name = p.ReadString();
                param.openGL = p.ReadString();
                param.directX = p.ReadString();
                shader.attributes[name] = param;
            }

            // Uniforms
            count = p.Read32();
            for (Uint32 i = 0; i < count; ++i)
            {
                UniformParameter param;
                auto name = p.ReadString();
                param.type = (Type::Type)p.Read8();
                param.openGL = p.ReadString();
                param.directX = p.ReadString();
                param.value = p.ReadString();
                shader.uniforms[name] = param;
            }

            // States
            count = p.Read32();
            for (Uint32 i = 0; i < count; ++i)
            {
                auto name = p.ReadString();
                auto value = p.ReadString();
                shader.deviceStates[name] = value;
            }
        }

        static void Write(Renderers::Effect::Shader const& shader, ByteArray& bin) // Used by Packet::Write<T>(T const&)
        {
            using namespace Renderers::Effect;
            auto getValue = [](boost::variant<Sampler, std::string> const& value)
                {
                    struct : public boost::static_visitor<std::string>
                    {
                        std::string operator()(Sampler const& sampler)
                        {
                            std::stringstream ss;
                            ss << "{";
                            for (auto const& s: sampler.states)
                                ss << s.first << "=" << s.second << ";";
                            ss << "}";
                            return ss.str();
                        }
                        std::string operator()(std::string const& s)
                        {
                            return s;
                        }
                    } visitor;
                    return boost::apply_visitor(visitor, value);
                };
            auto writeLongString = [&](std::string const& str) { bin.Write32((Uint32)str.size()); bin.WriteRawData(str.c_str(), (Uint32)str.size()); };

            // magic code: BIGBOOBS
            bin.Write32(0xB16B00B5); 

            // Shaders
            writeLongString(shader.source);
            writeLongString(shader.glslVertex);
            writeLongString(shader.glslPixel);
            writeLongString(shader.hlslVertex);
            writeLongString(shader.hlslPixel);

            // Attributes
            bin.Write32((Uint32)shader.attributes.size());
            for (auto const& attr: shader.attributes)
            {
                bin.WriteString(attr.first); // original name
                bin.WriteString(attr.second.openGL);
                bin.WriteString(attr.second.directX);
            }

            // Uniforms
            bin.Write32((Uint32)shader.uniforms.size());
            for (auto const& uniform: shader.uniforms)
            {
                bin.WriteString(uniform.first); // original name
                bin.Write8(uniform.second.type);
                bin.WriteString(uniform.second.openGL);
                bin.WriteString(uniform.second.directX);
                bin.WriteString(getValue(uniform.second.value)); // Default value
            }

            // States
            bin.Write32((Uint32)shader.deviceStates.size());
            for (auto const& state: shader.deviceStates)
            {
                bin.WriteString(state.first); // state name
                bin.WriteString(state.second); // value
            }
        }

        static std::unique_ptr<Renderers::Effect::Shader> Read(ByteArray const& p) // Used by Packet::Read<T>()
        {
            auto v = new Renderers::Effect::Shader();
            Read(p, *v);
            return std::unique_ptr<Renderers::Effect::Shader>(v);
        }
    };

}