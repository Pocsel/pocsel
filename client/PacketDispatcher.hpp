#ifndef __CLIENT_PACKETDISPATCHER_HPP__
#define __CLIENT_PACKETDISPATCHER_HPP__

#include <queue>
#include <unordered_map>

#include <boost/thread.hpp>

#include "tools/gui/EventManager.hpp"
#include "protocol/protocol.hpp"
#include "common/PacketPtr.hpp"

namespace Client {

    class PacketDispatcher
    {
    public:
        typedef boost::function<void(Common::Packet&)> PacketCallback;

    private:
        // Pour stocker des enum dans une std::unordered_map
        struct HashEnum
        {
            size_t operator ()(Protocol::ServerToClient action) const
            {
                return static_cast<size_t>(action);
            }
        };
        typedef std::unordered_map<Protocol::ServerToClient, PacketCallback, HashEnum> CallbackMap;

    private:
        Tools::Gui::EventManager& _eventManager;
        boost::mutex _mutex;
        std::queue<Common::Packet*> _toDispatch;
        volatile bool _alreadyNotified;
        int _dispatchHandler;
        CallbackMap _callbacks;

    public:
        PacketDispatcher(Tools::Gui::EventManager& eventManager);
        ~PacketDispatcher();
        void PushPacket(Common::PacketPtr& packet);
        void Register(Protocol::ServerToClient type, PacketCallback const& callback);
    private:
        void _DispatchPackets(Tools::Gui::Event const&);
    };

}

#endif
