#ifndef __TOOLS_RENDERERS_ARENDERER_HPP__
#define __TOOLS_RENDERERS_ARENDERER_HPP__

#include "tools/IRenderer.hpp"
#include "tools/Vector2.hpp"
#include "tools/Vector3.hpp"

namespace Tools { namespace Renderers {

    class ARenderer : public IRenderer
    {
    protected:
        struct RenderState
        {
            enum DrawState
            {
                None,
                Draw2D,
                Draw3D
            };

            DrawState state;
            IRenderTarget* target;

            glm::detail::tmat4x4<float> modelViewProjection;
            glm::detail::tmat4x4<float> model;
            glm::detail::tmat4x4<float> view;
            glm::detail::tmat4x4<float> projection;
            unsigned int matrixMode;
        };

        glm::uvec2 _screenSize;
        bool _fullscreen;
        std::list<std::function<void()>> _shutdownCallbacks;

        std::list<RenderState> _states;
        RenderState* _currentState;

        IShaderProgram* _currentProgram;

        glm::vec4 _clearColor;
        float _clearDepth;
        int _clearStencil;

    protected:
        ARenderer(glm::uvec2 const& screenSize, bool fullscreen) : _screenSize(screenSize), _fullscreen(fullscreen), _currentProgram(0) {}

    public:
        // Callbacks
        virtual void RegisterShutdownCallback(std::function<void()>&& callback) { this->_shutdownCallbacks.push_back(callback); }

        // Resources
        //virtual std::unique_ptr<Renderers::ITexture2D> CreateTexture2D(Renderers::PixelFormat::Type format, Uint32 size, void const* data, glm::uvec2 const& imgSize = glm::uvec2(0), void const* mipmapData = 0);
        //virtual std::unique_ptr<Renderers::ITexture2D> CreateTexture2D(std::string const& imagePath);
        //virtual std::unique_ptr<Renderers::IShaderProgram> CreateProgram(std::string const& effect);

        // Drawing
        virtual void UpdateCurrentParameters()
        {
            assert(this->_currentProgram != 0 && "Il faut obligatoirement un shader !");
            this->_currentProgram->UpdateParameter(ShaderParameterUsage::ModelMatrix);
            this->_currentProgram->UpdateParameter(ShaderParameterUsage::ViewMatrix);
            this->_currentProgram->UpdateParameter(ShaderParameterUsage::ProjectionMatrix);
            this->_currentProgram->UpdateParameter(ShaderParameterUsage::ModelViewMatrix);
            this->_currentProgram->UpdateParameter(ShaderParameterUsage::ViewProjectionMatrix);
            this->_currentProgram->UpdateParameter(ShaderParameterUsage::ModelViewProjectionMatrix);
            this->_currentProgram->UpdateCurrentPass();
        }

        // Matrices
        virtual void SetModelMatrix(glm::detail::tmat4x4<float> const& matrix)
        {
            this->_currentState->model = matrix;
            this->_currentState->modelViewProjection = this->_currentState->projection * this->_currentState->view * this->_currentState->model;
        }
        virtual void SetViewMatrix(glm::detail::tmat4x4<float> const& matrix)
        {
            this->_currentState->view = matrix;
            this->_currentState->modelViewProjection = this->_currentState->projection * this->_currentState->view * this->_currentState->model;
        }
        virtual void SetProjectionMatrix(glm::detail::tmat4x4<float> const& matrix)
        {
            this->_currentState->projection = matrix;
            this->_currentState->modelViewProjection = this->_currentState->projection * this->_currentState->view * this->_currentState->model;
        }
        glm::detail::tmat4x4<float> const& GetModelViewProjectionMatrix() const { return this->_currentState->modelViewProjection; }
        glm::detail::tmat4x4<float> const& GetModelMatrix() const { return this->_currentState->model; }
        glm::detail::tmat4x4<float> const& GetViewMatrix() const { return this->_currentState->view; }
        glm::detail::tmat4x4<float> const& GetProjectionMatrix() const { return this->_currentState->projection; }

        IShaderProgram& GetCurrentProgram() { return *this->_currentProgram; }
        void SetCurrentProgram(IShaderProgram& program) { this->_currentProgram = &program; }

    protected:
        void _GenericPushState(RenderState const& state, glm::mat4 const& projection = glm::mat4(1.0f))
        {
            RenderState rsOld;
            if (this->_states.size() > 0)
                rsOld = this->_states.front();

            this->_states.push_front(state);
            auto& rs = this->_states.front();
            this->_currentState = &rs;

            if (rs.target != 0)
            {
                rs.target->Bind();
                this->SetViewport(glm::uvec2(0, 0), rs.target->GetSize());
            }

            if (rs.state == RenderState::Draw2D)
            {
                rs.view = glm::translate<float>(0, 0, 1);
                glm::vec2 size(rs.target == 0 ? this->_screenSize : rs.target->GetSize());
                rs.projection = projection * glm::ortho<float>(0, size.x, size.y, 0);
            }
            else
            {
                rs.view = rsOld.view;
                rs.projection = rsOld.projection;
            }
            rs.model = rsOld.model;
            rs.modelViewProjection = rs.projection * rs.view * rs.model;
        }

        bool _GenericPopState()
        {
            auto rsOld = this->_states.front();
            this->_states.pop_front();
            auto& rs = this->_states.front();
            this->_currentState = &rs;

            if (rsOld.target != 0)
            {
                if (rs.target != 0)
                {
                    rs.target->Bind();
                    this->SetViewport(glm::uvec2(0, 0), rs.target->GetSize());
                }
                else
                {
                    for (auto it = this->_states.begin(), ite = this->_states.end(); it != ite; ++it)
                    {
                        if (it->target)
                        {
                            it->target->Bind();
                            this->SetViewport(glm::uvec2(0, 0), it->target->GetSize());
                            return false;
                        }
                    }
                    return true;
                }
            }
            return false;
        }
    };

}}

#endif