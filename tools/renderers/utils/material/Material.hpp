#ifndef __TOOLS_RENDERERS_UTILS_MATERIAL_MATERIAL_HPP__
#define __TOOLS_RENDERERS_UTILS_MATERIAL_MATERIAL_HPP__

#include "tools/IRenderer.hpp"
#include "tools/lua/Function.hpp"
#include "tools/lua/WeakRef.hpp"
#include "tools/renderers/utils/texture/ITexture.hpp"

namespace Tools { namespace Renderers { namespace Utils { namespace Material {
    class LuaMaterial;
    template<class T> class Variable;
}}}}

namespace Tools { namespace Renderers { namespace Utils { namespace Material {

    class IVariable :
        private boost::noncopyable
    {
    public:
        virtual ~IVariable() {}

        virtual Lua::Ref GetLuaValue(Lua::Interpreter& interpreter) const = 0;

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
        std::unique_ptr<Effect> _geometry;
        std::unique_ptr<Effect> _shadowMap;
        std::function<std::unique_ptr<Texture::ITexture>(std::string const&)> _loadTexture;
        std::map<IVariable const*, std::shared_ptr<Texture::ITexture>> _textures;
        std::map<std::string, std::unique_ptr<IVariable>> _variables;
        Lua::Ref _type;
        Lua::WeakRef _self;
        std::unique_ptr<Lua::Ref> _luaUpdate;
        bool _hasAlpha;

    public:
        Material(IRenderer& renderer,
            Lua::Ref const& type,
            Lua::Ref& materialSelf,
            std::function<IShaderProgram&(std::string const&)> const& loadShader,
            std::function<std::unique_ptr<Texture::ITexture>(std::string const&)> loadTexture);

        Material(Material const& material, Lua::Ref& materialSelf);

        template<class TValue>
        Variable<TValue>& GetVariable(std::string const& name);
        void SetTimeParameter(std::string const& name);
        void SetLuaUpdate(Lua::Ref const& update);
        void SetLuaMaterial(Lua::WeakRef const& ref) { this->_self = ref; }
        std::map<IVariable const*, std::shared_ptr<Texture::ITexture>> const& GetTextures() const { return this->_textures; }
        void SetTextures(IVariable const& variable, std::shared_ptr<Texture::ITexture> texture)
        {
            this->_textures[&variable] = texture;
        }

        void Update(Uint64 totalTime); // totalTime in microsecond
        void RenderGeometry(std::function<void()>& render, Uint64 totalTime); // bad performances
        void RenderShadowMap(std::function<void()>& render, Uint64 totalTime); // bad performances

        IShaderProgram& GetGeometryShader() { return this->_geometry->shader; }
        IShaderProgram& GetShadowMapShader() { return this->_shadowMap->shader; }
        std::map<std::string, std::unique_ptr<IVariable>> const& GetVariables() const { return this->_variables; }
        void UpdateParameters(int index, Uint64 totalTime);
        void UpdateParameters(IShaderProgram& shader, Uint64 totalTime);

        void SetLuaValue(std::string const& key, Lua::Ref value);
        bool HasAlpha() const { return this->_hasAlpha; }

        static void LoadLuaTypes(
            Lua::Interpreter& interpreter,
            std::function<std::unique_ptr<Texture::ITexture>(std::string const&)>&& loadTexture,
            std::function<std::unique_ptr<LuaMaterial>(std::string const&)>&& loadMaterial);
    private:
        Material& operator =(Material const& material);
        void _LoadVariables();
        template<class T>
        void _SetValue(std::string const& key, T value);
        template<class T>
        void _SetValue(std::string const& key, std::shared_ptr<T> value);
    };

}}}}

// Les methodes templates sont définies dans ce header:
#include "tools/renderers/utils/material/Variable.hpp"

#endif
