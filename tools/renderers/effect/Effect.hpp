#ifndef __TOOLS_RENDERERS_EFFECT_EFFECT_HPP__
#define __TOOLS_RENDERERS_EFFECT_EFFECT_HPP__

namespace Tools { namespace Renderers { namespace Effect {

    class Effect
    {
    private:
        std::unique_ptr<IShaderProgram> _program;
        RenderState _state;

    public:
        Effect(std::unique_ptr<IShaderProgram>&& program, RenderState const& rs);
    };

}}}

#endif
