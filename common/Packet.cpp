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

    void Packet::Dump() const
    {
        std::cout << "Packet dump (" << this->_size + 2 << " bytes total):" << std::endl;
        for (Uint16 i = 0; i < this->_size + 2; ++i)
        {
            std::cout << std::setfill('0') << std::setw(2) << std::hex << static_cast<Uint32>(this->_data[i - 2]);
            std::cout << " ";
        }
        std::cout << std::endl;
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

    void Packet::_Resize(Uint16 target)
    {
    //    assert(this->_allocSize < target);
        char* tmp = this->_data - 2;
        this->_data = new char[target + 2];
        std::memcpy(this->_data, tmp, this->_allocSize + 2);
    //    std::memset(this->_data + this->_allocSize, 42, target - this->_allocSize);
        this->_allocSize = target;
        Tools::DeleteTab(tmp);
        this->_data = this->_data + 2;
    }

    void Packet::_WriteSize()
    {
        char* tmp = this->_data - 2;
        tmp[0] = (this->_size & 0xFF00) >> 8;
        tmp[1] = this->_size & 0xFF;
    }

}
