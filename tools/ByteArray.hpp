#ifndef __TOOLS_BYTEARRAY_HPP__
#define __TOOLS_BYTEARRAY_HPP__

namespace Tools {

    class ByteArray
    {
    public:
        // In order to read/write custom classes into ByteArrays,
        // You must specialize ByteArray::Serializer<CustomClass>
        // with following methods :
        //  static std::unique_ptr<T> Read(ByteArray const&) // Used by ByteArray::Read<T>()
        //  static void Read(ByteArray const&, T&) // Used by ByteArray::Read<T>(T&)
        //  static void Write(T const&, ByteArray&) // Used by ByteArray::Write<T>(T const&)
        template<typename T> struct Serializer;

        // Implemented Plain old data serializer
        template<typename T> struct PodSerializer;

    protected:
        enum
        {
            Initialize = 1024,
            Sizestep = 512
        };

    protected:
        char* _data;
        Uint32 _size;
        Uint32 _allocSize;
        mutable Uint32 _offset;

    public:
        ByteArray();
        ByteArray(ByteArray&& byteArray) : // move cstor, inline for performance
            _data(byteArray._data),
            _size(byteArray._size),
            _allocSize(byteArray._allocSize),
            _offset(byteArray._offset)
        {
            byteArray._data = 0;
        }
        explicit ByteArray(ByteArray const& ByteArray);
        explicit ByteArray(std::vector<char> const& vector);
        ByteArray(char const* data, Uint32 size);
        virtual ~ByteArray();

        ByteArray& operator =(ByteArray const& ByteArray);
        Uint32 GetBytesLeft() const;
        void Clear();
        void ResetOffset();
        void SetData(char const* data, Uint32 size);
        void Write8(Uint8 val);
        void Write16(Uint16 val);
        void Write32(Uint32 val);
        void Write64(Uint64 val);
        void WriteBool(bool val);
        void WriteFloat(float val);
        void WriteDouble(double val);
        void WriteString(std::string const& val);
        void WriteData(std::vector<char> const& val);
        void WriteData(void const* data, Uint16 size);
        void WriteRawData(void const* data, Uint32 size);
        Uint8 Read8() const;
        Uint16 Read16() const;
        Uint32 Read32() const;
        Uint64 Read64() const;
        bool ReadBool() const;
        float ReadFloat() const;
        double ReadDouble() const;
        std::string ReadString() const;
        std::vector<char> ReadData() const;
        char const* ReadRawData(Uint32 size) const;
        Uint32 GetSize() const { return this->_size; }
        char const* GetData() const { return this->_data; }

        template<typename T>
        inline void Write(T const& o)
        {
            Serializer<T>::Write(o, *this);
        }

        template<typename T>
        inline void Read(T& o) const
        {
            Serializer<T>::Read(*this, o);
        }
        template<typename T>
        inline std::unique_ptr<T> Read() const
        {
            return Serializer<T>::Read(*this);
        }

#define BYTEARRAY_GEN_METHODS(Type, ReadMethod, WriteMethod) \
        inline void Write(Type o) { this->WriteMethod(o); }\
        inline void Read(Type& o) const { o = this->ReadMethod(); } \

        BYTEARRAY_GEN_METHODS(Uint8, Read8, Write8)
        BYTEARRAY_GEN_METHODS(Int8, Read8, Write8)
        BYTEARRAY_GEN_METHODS(Uint16, Read16, Write16)
        BYTEARRAY_GEN_METHODS(Int16, Read16, Write16)
        BYTEARRAY_GEN_METHODS(Uint32, Read32, Write32)
        BYTEARRAY_GEN_METHODS(Int32, Read32, Write32)
        BYTEARRAY_GEN_METHODS(Uint64, Read64, Write64)
        BYTEARRAY_GEN_METHODS(Int64, Read64, Write64)
        BYTEARRAY_GEN_METHODS(float, ReadFloat, WriteFloat)
        BYTEARRAY_GEN_METHODS(double, ReadDouble, WriteDouble)
        BYTEARRAY_GEN_METHODS(bool, ReadBool, WriteBool)
        BYTEARRAY_GEN_METHODS(std::string, ReadString, WriteString)
#undef BYTEARRAY_GEN_METHODS

    protected:
        virtual void _Resize(Uint32 target);
        void _PrepareWrite(Uint32 bytesCount)
        {
            if (this->_offset + bytesCount > this->_allocSize)
                this->_Resize(this->_size + bytesCount);
            if (this->_offset + bytesCount > this->_size)
                this->_size = this->_offset + bytesCount;
        }
    };

}

#endif
