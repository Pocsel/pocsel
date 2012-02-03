#include <stdexcept>
#include <cassert>
#include <cstring>
#include <iomanip>
#include <iostream>

#include "tools/ByteArray.hpp"

namespace Tools {

    ByteArray::ByteArray() :
        _size(0), _allocSize(INITIALSIZE), _offset(0)
    {
        this->_data = new char[INITIALSIZE];
    }

    ByteArray::ByteArray(ByteArray const& ByteArray) :
        _size(ByteArray._size), _allocSize(ByteArray._allocSize), _offset(ByteArray._offset)
    {
        this->_Resize(this->_allocSize);
        ::memcpy(this->_data, ByteArray._data, this->_size);
    }

    ByteArray::~ByteArray()
    {
        delete [] this->_data;
    }

    ByteArray& ByteArray::operator =(ByteArray const& ByteArray)
    {
        if (this == &ByteArray)
            return *this;
        this->_size = ByteArray._size;
        this->_allocSize = ByteArray._allocSize;
        this->_offset = ByteArray._offset;
        this->_Resize(this->_allocSize);
        ::memcpy(this->_data, ByteArray._data, this->_size);
        return (*this);
    }

    Uint16 ByteArray::GetBytesLeft() const
    {
        return this->_size - this->_offset;
    }

    void ByteArray::Write8(Uint8 val)
    {
        this->_PrepareWrite(1);
        this->_data[this->_offset++] = val;
    }

    void ByteArray::Write16(Uint16 val)
    {
        this->_PrepareWrite(2);
        this->_data[this->_offset++] = (val >> 8) & 0xFF;
        this->_data[this->_offset++] = val & 0xFF;
    }

    void ByteArray::Write32(Uint32 val)
    {
        this->_PrepareWrite(4);
        this->_data[this->_offset++] = (val >> 24) & 0xFF;
        this->_data[this->_offset++] = (val >> 16) & 0xFF;
        this->_data[this->_offset++] = (val >> 8) & 0xFF;
        this->_data[this->_offset++] = val & 0xFF;
    }

    void ByteArray::Write64(Uint64 val)
    {
        this->_PrepareWrite(8);
        this->_data[this->_offset++] = (val >> 56) & 0xFF;
        this->_data[this->_offset++] = (val >> 48) & 0xFF;
        this->_data[this->_offset++] = (val >> 40) & 0xFF;
        this->_data[this->_offset++] = (val >> 32) & 0xFF;
        this->_data[this->_offset++] = (val >> 24) & 0xFF;
        this->_data[this->_offset++] = (val >> 16) & 0xFF;
        this->_data[this->_offset++] = (val >> 8) & 0xFF;
        this->_data[this->_offset++] = val & 0xFF;
    }

    void ByteArray::WriteBool(bool val)
    {
        this->Write8(static_cast<Uint8>(val));
    }

    void ByteArray::WriteFloat(float val)
    {
        static_assert(sizeof(float) == 4, "c'est pas IEEE 754 :(");
        this->_PrepareWrite(sizeof(float));
        std::memcpy(this->_data + this->_offset, &val, sizeof(float));
        this->_offset += sizeof(float);
    }

    void ByteArray::WriteDouble(double val)
    {
        static_assert(sizeof(double) == 8, "c'est pas IEEE 754 :(");
        this->_PrepareWrite(sizeof(double));
        std::memcpy(this->_data + this->_offset, &val, sizeof(double));
        this->_offset += sizeof(double);
    }

    void ByteArray::WriteString(std::string const& val)
    {
        Uint16 size = val.size();
        this->Write16(size);
        this->_PrepareWrite(size);
        ::memcpy(this->_data + this->_offset, val.data(), size);
        this->_offset += val.size();
    }

    void ByteArray::WriteData(std::vector<char> const& val)
    {
        Uint16 size = val.size();
        this->Write16(size);
        this->_PrepareWrite(size);
        ::memcpy(this->_data + this->_offset, val.data(), size);
        this->_offset += size;
    }

    void ByteArray::WriteData(void const* data, Uint16 size)
    {
        this->Write16(size);
        this->_PrepareWrite(size);
        ::memcpy(this->_data + this->_offset, data, size);
        this->_offset += size;
    }

    void ByteArray::WriteRawData(void const* data, Uint16 size)
    {
        this->_PrepareWrite(size);
        ::memcpy(this->_data + this->_offset, data, size);
        this->_offset += size;
    }

    Uint8 ByteArray::Read8() const
    {
        if (this->_offset < this->_size)
            return this->_data[this->_offset++];
        throw std::runtime_error("ByteArray too small to read");
    }

    Uint16 ByteArray::Read16() const
    {
        if (this->_offset + 1 < this->_size)
        {
            Uint16 ret = (this->_data[this->_offset++] << 8) & 0xFF00;
            ret |= this->_data[this->_offset++] & 0x00FF;
            return ret;
        }
        throw std::runtime_error("ByteArray too small to read");
    }

    Uint32 ByteArray::Read32() const
    {
        if (this->_offset + 3 < this->_size)
        {
            Uint32 ret = (this->_data[this->_offset++] << 24) & 0xFF000000;
            ret |= (this->_data[this->_offset++] << 16) & 0x00FF0000;
            ret |= (this->_data[this->_offset++] << 8) & 0x0000FF00;
            ret |= this->_data[this->_offset++] & 0x000000FF;
            return ret;
        }
        throw std::runtime_error("ByteArray too small to read");
    }

    Uint64 ByteArray::Read64() const
    {
        if (this->_offset + 7 < this->_size)
        {
            Uint64 ret = (static_cast<Uint64>(this->_data[this->_offset++]) << 56) & 0xFF00000000000000ull;
            ret |= (static_cast<Uint64>(this->_data[this->_offset++]) << 48) & 0x00FF000000000000ull;
            ret |= (static_cast<Uint64>(this->_data[this->_offset++]) << 40) & 0x0000FF0000000000ull;
            ret |= (static_cast<Uint64>(this->_data[this->_offset++]) << 32) & 0x000000FF00000000ull;
            ret |= (static_cast<Uint64>(this->_data[this->_offset++]) << 24) & 0x00000000FF000000;
            ret |= (static_cast<Uint64>(this->_data[this->_offset++]) << 16) & 0x0000000000FF0000;
            ret |= (static_cast<Uint64>(this->_data[this->_offset++]) << 8) & 0x000000000000FF00;
            ret |= static_cast<Uint64>(this->_data[this->_offset++]) & 0x00000000000000FF;
            return ret;
        }
        throw std::runtime_error("ByteArray too small to read");
    }

    bool ByteArray::ReadBool() const
    {
        Uint8 res = this->Read8();
        if (res != 0 && res != 1)
            throw std::runtime_error("Wrong boolean value");
        return res != 0;
    }

    float ByteArray::ReadFloat() const
    {
        if (this->_offset + sizeof(float) <= this->_size)
        {
            float const* ret = reinterpret_cast<float const*>(&this->_data[this->_offset]);
            this->_offset += sizeof(float);
            return *ret;
        }
        throw std::runtime_error("ByteArray too small to read");
    }

    double ByteArray::ReadDouble() const
    {
        if (this->_offset + sizeof(double) <= this->_size)
        {
            double const* ret = reinterpret_cast<double const*>(&this->_data[this->_offset]);
            this->_offset += sizeof(double);
            return *ret;
        }
        throw std::runtime_error("ByteArray too small to read");
    }

    std::string ByteArray::ReadString() const
    {
        Uint16 size = this->Read16();
        std::string ret;
        for (Uint32 i = 0; i < size; ++i)
            ret += this->Read8();
        return ret;
    }

    std::vector<char> ByteArray::ReadData() const
    {
        Uint16 size = this->Read16();
        std::vector<char> data(size);
        for (Uint16 i = 0; i < size; ++i)
            data[i] = this->Read8();
        return data;
    }

    char const* ByteArray::ReadRawData(Uint16 size) const
    {
        if (this->_offset + size <= this->_size)
        {
            char const* ret = this->_data + this->_offset;
            this->_offset += size;
            return ret;
        }
        throw std::runtime_error("ByteArray too small to read");
    }

    void ByteArray::Clear()
    {
        this->_size = 0;
        this->_offset = 0;
    }

    void ByteArray::SetData(char const* data, Uint16 size)
    {
        if (this->_allocSize < size)
            this->_Resize(size);
        ::memcpy(this->_data, data, size);
        this->_size = size;
        this->_offset = 0;
    }

    void ByteArray::_Resize(Uint16 target)
    {
        assert(this->_allocSize < target);
        char* tmp = this->_data;
        this->_data = new char[target];
        std::memcpy(this->_data, tmp, this->_allocSize);
    //    std::memset(this->_data + this->_allocSize, 42, target - this->_allocSize);
        this->_allocSize = target;
        delete [] tmp;
    }

    void ByteArray::_PrepareWrite(Uint16 bytesCount)
    {
        if (this->_offset + bytesCount >= this->_allocSize)
            this->_Resize(this->_allocSize + ((bytesCount / SIZESTEP + 1) * SIZESTEP));
        if (this->_offset + bytesCount > this->_size)
            this->_size = this->_offset + bytesCount;
    }

    char const* ByteArray::GetData() const
    {
        return this->_data;
    }

    Uint16 ByteArray::GetSize() const
    {
        return this->_size;
    }

}
