#pragma once
#include "tools/precompiled.hpp"

#include "tools/ByteArray.hpp"
#include "tools/gfx/effect/Shader.hpp"

namespace Tools {

    template<>
    struct ByteArray::Serializer<Gfx::Effect::CompleteShader>
    {
        static void Read(ByteArray const& p, Gfx::Effect::CompleteShader& shader)  // Used by Packet::Read<T>(T&)
        {
            using namespace Gfx::Effect;
            auto readLongString = [&]() { auto s = p.Read32(); return std::string(p.ReadRawData(s), s); };

            // magic code
            if (p.Read32() != 0xB16B00B5)
                throw std::runtime_error("Where is bigboobs ?!");

            auto readShader = [&](Shader& shader) {
                    // shaders
                    shader.source = readLongString();
                    shader.vertex = readLongString();
                    shader.pixel = readLongString();

                    // Attributes
                    auto count = p.Read32();
                    for (Uint32 i = 0; i < count; ++i)
                    {
                        BaseParameter param;
                        param.semantic = Semantic::Type(p.Read16());
                        auto name = p.ReadString();
                        param.name = p.ReadString();
                        shader.attributes[name] = param;
                    }

                    // Uniforms
                    count = p.Read32();
                    for (Uint32 i = 0; i < count; ++i)
                    {
                        UniformParameter param;
                        param.semantic = Semantic::Type(p.Read16());
                        auto name = p.ReadString();
                        param.type = (Type::Type)p.Read8();
                        param.name = p.ReadString();
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
                };

            // OpenGL
            readShader(shader.openGL);
            // DirectX
            readShader(shader.directX);
        }

        static std::unique_ptr<Gfx::Effect::CompleteShader> Read(ByteArray& p)
        {
            return std::unique_ptr<Gfx::Effect::CompleteShader>(new Gfx::Effect::CompleteShader(p));
        }

    private:
        static void Write(Gfx::Effect::CompleteShader const& shader, ByteArray& bin);
    };

}