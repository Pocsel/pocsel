#ifndef __TOOLS_SOUND_ACHANNEL_HPP__
#define __TOOLS_SOUND_ACHANNEL_HPP__

namespace Tools { namespace Sound {

    class AChannel :
        private boost::noncopyable
    {
    public:
        virtual ~AChannel() {};
        void Play() { this->SetPaused(false); }
        void Pause() { this->SetPaused(true); }
        virtual void SetPaused(bool paused) = 0;
        virtual void SetPosition(glm::fvec3 const& pos) = 0;
        virtual void SetVelocity(glm::fvec3 const& vel /* units per second */) = 0;
    };

}}

#endif
