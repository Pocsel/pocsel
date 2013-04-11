#ifndef __TOOLS_SOUND_ISOUND_HPP__
#define __TOOLS_SOUND_ISOUND_HPP__

namespace Tools { namespace Sound {

    class AChannel;

    class ISound :
        private boost::noncopyable
    {
    public:
        virtual ~ISound() {};
        virtual std::unique_ptr<AChannel> GetChannel() const = 0;
    };

}}

#endif
