#ifndef __TOOLS_SOUND_ISOUND_HPP__
#define __TOOLS_SOUND_ISOUND_HPP__

namespace Tools { namespace Sound {

    class ISound :
        private boost::noncopyable
    {
    public:
        virtual ~ISound() {};
        virtual void Play() const = 0;
    };

}}

#endif
