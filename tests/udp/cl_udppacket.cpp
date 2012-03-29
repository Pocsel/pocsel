#include "cl_udppacket.hpp"

namespace cl {

    UdpPacket::UdpPacket(Uint32 id) :
        Common::Packet(),
        forceUdp(false),
        _id(id)
    {
        this->_data = this->_data + (sizeof(Uint32) - 2);
        this->_allocSize -= sizeof(Uint32) - 2;
    }

    UdpPacket::~UdpPacket()
    {
        this->_data -= sizeof(Uint32) - 2;
    }

    void UdpPacket::_Resize(Uint16 target)
    {
        char* tmp = this->_data - sizeof(Uint32);
        this->_data = new char[target + sizeof(Uint32)];
        std::memcpy(this->_data, tmp, this->_allocSize + sizeof(Uint32));
        this->_allocSize = target;
        Tools::DeleteTab(tmp);
        this->_data = this->_data + sizeof(Uint32);
    }

    char const* UdpPacket::GetCompleteData() const
    {
        const_cast<UdpPacket*>(this)->_WriteId();
        return this->_data - sizeof(Uint32);
    }

    Uint16 UdpPacket::GetCompleteSize() const
    {
        return (Uint16)(this->_size + sizeof(Uint32));
    }

    void UdpPacket::_WriteId()
    {
        char* tmp = this->_data - sizeof(Uint32);
        tmp[0] = (this->_id & 0xFF000000) >> 24;
        tmp[1] = (this->_id & 0x00FF0000) >> 16;
        tmp[2] = (this->_id & 0x0000FF00) >> 8;
        tmp[3] = (this->_id & 0x000000FF);
    }

}
