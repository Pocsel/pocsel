#ifndef __TOOLS_RENDERERS_UTILS_MATERIAL_MATERIAL_HPP__
#define __TOOLS_RENDERERS_UTILS_MATERIAL_MATERIAL_HPP__

#include "tools/IRenderer.hpp"
#include "tools/lua/Function.hpp"
#include "tools/renderers/utils/texture/ITexture.hpp"

namespace Tools { namespace Renderers { namespace Utils { namespace Material {
    template<class T> class Variable;
}}}}

namespace Tools { namespace Renderers { namespace Utils { namespace Material {

    class IVariable :
        private boost::noncopyable
    {
    public:
        virtual ~IVariable() {}

        virtual void UpdateParameter(int index) = 0;
    };

    class Material
    {
    private:
        struct Effect
        {
            IShaderProgram& shader;
            IShaderParameter* totalTime;

            Effect(IShaderProgram& shader) : shader(shader), totalTime(0) {}
            void UpdateParameters(Uint64 totalTime);
        };

        IRenderer& _renderer;
        Effect _geometry;
        Effect _shadowMap;
        std::map<IVariable const*, std::shared_ptr<Texture::ITexture>> _textures;
        std::map<std::string, std::unique_ptr<IVariable>> _variables;
        Lua::Ref _luaMaterial;
        std::unique_ptr<Lua::Ref> _luaUpdate;

    public:
        Material(IRenderer& renderer, Lua::Ref const& material, IShaderProgram& geometry, IShaderProgram& shadowMap);

        explicit Material(Material const& material);

        template<class TValue>
        Variable<TValue>& GetVariable(std::string const& name);
        void SetTimeParameter(std::string const& name);
        void SetLuaUpdate(Lua::Ref const& update);
        void SetLuaMaterial(Lua::Ref const& ref) { this->_luaMaterial = ref; }
        std::map<IVariable const*, std::shared_ptr<Texture::ITexture>> const& GetTextures() const { return this->_textures; }
        void SetTextures(IVariable const& variable, std::shared_ptr<Texture::ITexture> texture)
        {
            this->_textures[&variable] = texture;
        }

        void Update(Uint64 totalTime); // totalTime in microsecond
        void RenderGeometry(std::function<void()>& render, Uint64 totalTime); // bad performances
        void RenderShadowMap(std::function<void()>& render, Uint64 totalTime); // bad performances

        IShaderProgram& GetGeometryShader() { return this->_geometry.shader; }
        IShaderProgram& GetShadowMapShader() { return this->_shadowMap.shader; }
        std::map<std::string, std::unique_ptr<IVariable>> const& GetVariables() const { return this->_variables; }
        void UpdateParameters(int index, Uint64 totalTime);
        void UpdateParameters(IShaderProgram& shader, Uint64 totalTime);
    private:
        Material& operator =(Material const& material);
    };

}}}}

#include "tools/renderers/utils/material/Variable.hpp"

#endif