#ifndef __TOOLS_SOUND_ISOUNDSYSTEM_HPP__
#define __TOOLS_SOUND_ISOUNDSYSTEM_HPP__

namespace Common {
    class Resource;
}

namespace Tools { namespace Sound {

    class ISound;

    class ISoundSystem :
        private boost::noncopyable
    {
    public:
        virtual ~ISoundSystem() {};
        virtual void Update() = 0; // once per frame for 3D calculations, callbacks, ...
        virtual void SetEars(glm::fvec3 const& pos, glm::fvec3 const& vel /* units per second */, glm::fvec3 const& forward, glm::fvec3 const& up) = 0;
        virtual void SetEars(glm::fvec3 const& pos, glm::fvec3 const& vel /* units per second */, glm::fvec3 const& forward) = 0;
        virtual void SetEars(glm::fvec3 const& pos, glm::fvec3 const& vel /* units per second */) = 0;
        virtual void SetEars(glm::fvec3 const& pos) = 0;
        virtual void SetEarsOrientation(glm::fvec3 const& forward, glm::fvec3 const& up) = 0;
        virtual ISound* CreateSound(std::string const& path) const = 0; // decompress entire file from disk to memory
        virtual ISound* CreateSound(Common::Resource const& data) const = 0; // decompress entire data to memory (no ownership needed)
        virtual ISound* CreateSound(std::unique_ptr<Common::Resource> data) const = 0; // streaming from compressed & owned data (for big sounds/music)
    };

}}

#endif
