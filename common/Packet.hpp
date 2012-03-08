#ifndef __COMMON_PACKET_HPP__
#define __COMMON_PACKET_HPP__

#include "tools/ByteArray.hpp"

namespace Common {

    template<class T>
    inline Uint16 ArrayToUint16(T const& array)
    {
        static_assert(sizeof(array[0]) == 1, "cette fonction prend un tableau de char, bande de cons !");
        return ((static_cast<Uint16>(array[0]) << 8) & 0xFF00) | (array[1] & 0x00FF);
    }

    class Packet : public Tools::ByteArray
    {
    public:
        explicit Packet();
        explicit Packet(Packet const& packet);
        virtual ~Packet();

        Uint16 GetCompleteSize() const;
        char const* GetCompleteData() const;

    protected:
        void _WriteSize();
        virtual void _Resize(Uint32 target);
    };

}

#endif
