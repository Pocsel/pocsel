#include "common/Packet.hpp"

namespace Common {

    Packet::Packet() :
        Tools::ByteArray()
    {
        this->_data = this->_data + 2;
        this->_allocSize -= 2;
    }

    Packet::Packet(Packet const& packet) :
        Tools::ByteArray()
    {
        this->_Resize(packet._allocSize);
        ::memcpy(this->_data - 2, packet._data - 2, packet._allocSize + 2);
        this->_allocSize = packet._allocSize;
        this->_size = packet._size;
        this->_offset = packet._offset;
    }

    Packet::~Packet()
    {
        this->_data -= 2;
    }

    char const* Packet::GetCompleteData() const
    {
        const_cast<Packet*>(this)->_WriteSize();
        return this->_data - 2;
    }

    Uint16 Packet::GetCompleteSize() const
    {
        return this->_size + 2;
    }

    void Packet::_Resize(Uint32 target)
    {
        if (target > 0xffff - 2)
            throw std::runtime_error("Trying to create a too big packet");

        char* tmp = this->_data - 2;
        this->_data = new char[target + 2];
        std::memcpy(this->_data, tmp, this->_allocSize + 2);
        this->_allocSize = target;
        Tools::DeleteTab(tmp);
        this->_data = this->_data + 2;
    }

    void Packet::_WriteSize()
    {
        assert(this->_size <= 0xffff - 2);
        char* tmp = this->_data - 2;
        tmp[0] = (this->_size & 0xFF00) >> 8;
        tmp[1] = this->_size & 0x00FF;
    }

}
