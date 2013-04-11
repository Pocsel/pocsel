#include "tools/sound/fmod/Channel.hpp"

namespace Tools { namespace Sound { namespace Fmod {

    Channel::Channel(FMOD::Channel* channel) :
        _channel(channel)
    {
    }

    void Channel::SetPaused(bool paused)
    {
        this->_channel->setPaused(paused);
    }

    void Channel::SetPosition(glm::fvec3 const& pos)
    {
        this->_pos.x = pos.x;
        this->_pos.y = pos.y;
        this->_pos.z = pos.z;
        this->_channel->set3DAttributes(&this->_pos, nullptr /* velocity */);
    }

    void Channel::SetVelocity(glm::fvec3 const& vel)
    {
        this->_vel.x = vel.x;
        this->_vel.y = vel.y;
        this->_vel.z = vel.z;
        this->_channel->set3DAttributes(nullptr /* position */, &this->_vel);
    }

}}}
