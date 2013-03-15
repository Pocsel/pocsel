#ifndef __CLIENT_GRAPHICS_LIGHTMANAGER_HPP__
#define __CLIENT_GRAPHICS_LIGHTMANAGER_HPP__

#include "tools/gfx/utils/light/LightRenderer.hpp"
#include "common/Position.hpp"

namespace Tools {
    namespace Lua {
        class Interpreter;
    }
    namespace Gfx {
        class IRenderer;
        namespace Utils {
            class GBuffer;
        }
    }
}

namespace Common {
    struct Camera;
}

namespace Client { namespace Game {
    class Game;
}}

namespace Client { namespace Graphics {

    class LightManager : private boost::noncopyable
    {
    public:
        class DirectionnalLight : private boost::noncopyable
        {
        private:
            LightManager& _lightManager;
            Tools::Gfx::Utils::Light::DirectionnalLight _internal;
        public:
            bool visible;

        public:
            DirectionnalLight(LightManager& _lightManager);
            ~DirectionnalLight() { this->_lightManager._Unregister(*this); }

            glm::vec3 const& GetAmbientColor() const { return this->_internal.ambientColor; }
            glm::vec3 const& GetDiffuseColor() const { return this->_internal.diffuseColor; }
            glm::vec3 const& GetSpecularColor() const { return this->_internal.specularColor; }
            glm::vec3 const& GetDirection() const { return this->_internal.direction; }
            void SetAmbientColor(glm::vec3 color) { this->_internal.ambientColor = color; }
            void SetDiffuseColor(glm::vec3 color) { this->_internal.diffuseColor = color; }
            void SetSpecularColor(glm::vec3 color) { this->_internal.specularColor = color; }
            void SetDirection(glm::vec3 direction) { this->_internal.direction = direction; }

            Tools::Gfx::Utils::Light::DirectionnalLight& GetInternal() { return this->_internal; }
        };

        class PointLight : private boost::noncopyable
        {
        private:
            LightManager& _lightManager;
            Tools::Gfx::Utils::Light::PointLight _internal;
        public:
            Common::Position position;
            bool visible;

        public:
            PointLight(LightManager& lightManager);
            ~PointLight() { this->_lightManager._Unregister(*this); }

            glm::vec3 const& GetDiffuseColor() const { return this->_internal.diffuseColor; }
            glm::vec3 const& GetSpecularColor() const { return this->_internal.specularColor; }
            float GetRange() const { return this->_internal.range; }
            void SetDiffuseColor(glm::vec3 color) { this->_internal.diffuseColor = color; }
            void SetSpecularColor(glm::vec3 color) { this->_internal.specularColor = color; }
            void SetRange(float range) { this->_internal.range = range; }

            Tools::Gfx::Utils::Light::PointLight& GetInternal() { return this->_internal; }
        };

    private:
        Game::Game& _game;
        Tools::Gfx::IRenderer& _renderer;
        Tools::Gfx::Utils::Light::LightRenderer _lightRenderer;
        std::list<std::unique_ptr<DirectionnalLight>> _directionnals;
        std::list<std::unique_ptr<PointLight>> _points;

    public:
        LightManager(Game::Game& game);

        void Render(Common::Camera const& camera, Tools::Gfx::Utils::GBuffer& gbuffer);
        DirectionnalLight& CreateDirectionnalLight();
        PointLight& CreatePointLight();

    private:
        void _Unregister(DirectionnalLight& light);
        void _Unregister(PointLight& light);
        void _RegisterLua(Tools::Lua::Interpreter& interpreter);
    };

}}

#endif
