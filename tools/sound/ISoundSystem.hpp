#ifndef __TOOLS_SOUND_ISOUNDSYSTEM_HPP__
#define __TOOLS_SOUND_ISOUNDSYSTEM_HPP__

namespace Tools { namespace Sound {

    class ISound;

    class ISoundSystem :
        private boost::noncopyable
    {
    public:
        virtual ~ISoundSystem() {};
        virtual void Update() const = 0;
    };

}}

#endif
