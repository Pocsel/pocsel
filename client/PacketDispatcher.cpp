#include <cassert>
#include <iostream>

#include "tools/ToString.hpp"

#include "common/Packet.hpp"

#include "PacketDispatcher.hpp"

using namespace Client;

PacketDispatcher::PacketDispatcher(Tools::Gui::EventManager& eventManager) :
    _eventManager(eventManager),
    _mutex(),
    _toDispatch(),
    _alreadyNotified(false),
    _dispatchHandler(0)

{
    this->_dispatchHandler = this->_eventManager.Connect(
        "network/has-packet",
        std::bind(&PacketDispatcher::_DispatchPackets, this, std::placeholders::_1)
    );
}

PacketDispatcher::~PacketDispatcher()
{
    this->_eventManager.Disconnect(this->_dispatchHandler);
    boost::unique_lock<boost::mutex> lock(this->_mutex);
    while (this->_toDispatch.size())
    {
        delete this->_toDispatch.front();
        this->_toDispatch.pop();
    }
}

void PacketDispatcher::PushPacket(Common::PacketPtr& packet)
{
//    std::cout << "PushPacket\n";
    assert(packet.get() != 0 && "Empty packet given");
    boost::unique_lock<boost::mutex> lock(this->_mutex);
    this->_toDispatch.push(packet.release());
    if (!this->_alreadyNotified)
    {
        this->_alreadyNotified = true;
        this->_eventManager.Notify<Tools::Gui::Event>("network/has-packet");
    }
}


void PacketDispatcher::_DispatchPackets(Tools::Gui::Event const&)
{
//    std::cout << "_DispatchPackets\n";
    std::queue<Common::Packet*> toDispatchCopy;

    {
        boost::unique_lock<boost::mutex> lock(this->_mutex);
        toDispatchCopy = this->_toDispatch;
        this->_toDispatch = std::queue<Common::Packet*>();
        this->_alreadyNotified = false;
    }

    while (toDispatchCopy.size())
    {
        Common::Packet* p = toDispatchCopy.front();
        toDispatchCopy.pop();
        // use p here
        assert(p != 0 && "null packet stored");
        static_assert(sizeof(Protocol::ActionType) == 1, "faut changer le packet.Read8()");
        Protocol::ServerToClient type = static_cast<Protocol::ServerToClient>(p->Read8());
        auto callback = this->_callbacks.find(type);
        if (callback == this->_callbacks.end())
            throw std::runtime_error(Tools::ToString(type) + " packet type is not registered!");
        if (type != Protocol::ServerToClient::Chunk)
            std::cout << "PACKET >>> " << Tools::ToString(type) << " <<< RECEIVED\n";
        callback->second(*p);
        delete p;
    }
}

void PacketDispatcher::Register(Protocol::ServerToClient type, PacketCallback const& callback)
{
    if (this->_callbacks.find(type) != this->_callbacks.end())
        throw std::runtime_error(Tools::ToString(type) + " packet type is already registered");
    this->_callbacks[type] = callback;
}
