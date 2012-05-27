#include "common/Packet.hpp"

namespace Common {

    Packet::Packet() :
        Tools::ByteArray()
    {
        this->_data = this->_data + SizeBytes;
        this->_allocSize -= SizeBytes;
    }

    Packet::Packet(Packet const& packet) :
        Tools::ByteArray()
    {
        this->_data = this->_data + SizeBytes;
        this->_allocSize -= SizeBytes;
        this->_Resize(packet._allocSize);
        ::memcpy(this->_data - SizeBytes, packet._data - SizeBytes, packet._size + SizeBytes);
        this->_allocSize = packet._allocSize;
        this->_size = packet._size;
        this->_offset = packet._offset;
    }

    Packet::~Packet()
    {
        this->_data -= SizeBytes;
    }

    char const* Packet::GetCompleteData() const
    {
        const_cast<Packet*>(this)->_WriteSize();
        return this->_data - SizeBytes;
    }

    Uint16 Packet::GetCompleteSize() const
    {
        return this->_size + SizeBytes;
    }

    void Packet::_Resize(Uint32 target)
    {
        if (target > MaxSize)
        {
            std::cout << target;
            throw std::runtime_error("Trying to create a too big packet");
        }

        Uint32 newTarget = (target / Sizestep + 1) * Sizestep;
        if (newTarget > MaxSize)
            newTarget = MaxSize;
        target = newTarget;

        char* tmp = this->_data - SizeBytes;
        this->_data = new char[target + SizeBytes];
        std::memcpy(this->_data, tmp, this->_allocSize + SizeBytes);
        this->_allocSize = target;
        Tools::DeleteTab(tmp);
        this->_data = this->_data + SizeBytes;
    }

    void Packet::_WriteSize()
    {
        static_assert(SizeBytes == 2, "faut changer du code ici si on change la taille des packets");
        assert(this->_size <= MaxSize);
        char* tmp = this->_data - SizeBytes;
        tmp[0] = (this->_size & 0xFF00) >> 8;
        tmp[1] = this->_size & 0x00FF;
    }

}
