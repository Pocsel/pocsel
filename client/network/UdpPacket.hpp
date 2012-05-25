#ifndef __CLIENT_NETWORK_UDPPACKET_HPP__
#define __CLIENT_NETWORK_UDPPACKET_HPP__

#include "common/Packet.hpp"

namespace Client { namespace Network {

    class UdpPacket :
        public Common::Packet
    {
    public:
        bool forceUdp;
    private:
        Uint32 _id;
    public:
        explicit UdpPacket(Uint32 id, bool forceUdp = false);
        virtual ~UdpPacket();

        void ConvertToTcpPacket();

        Uint16 GetCompleteSize() const;
        char const* GetCompleteData() const;
    protected:
        virtual void _Resize(Uint32 target);
        void _WriteId();
    };

}}

#endif
