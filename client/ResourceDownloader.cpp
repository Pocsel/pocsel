#include <iostream>

#include "ResourceDownloader.hpp"
#include "PacketDispatcher.hpp"
#include "Network.hpp"
#include "common/Packet.hpp"
#include "common/Resource.hpp"
#include "PacketExtractor.hpp"
#include "PacketCreator.hpp"
#include "CacheDatabaseProxy.hpp"
#include "tools/gui/EventManager.hpp"

using namespace Client;

ResourceDownloader::ResourceDownloader(Tools::Gui::EventManager& eventManager,
        Network& network,
        PacketDispatcher& packetDispatcher,
        CacheDatabaseProxy& database) :
    _eventManager(eventManager),
    _database(database),
    _network(network),
    _packetDispatcher(packetDispatcher),
    _nbNeededResources(0),
    _nbDownloadedResources(0)
{
    this->_packetDispatcher.Register(
        Protocol::ServerToClient::NeededResourceIds,
        std::bind(&ResourceDownloader::_HandleNeededResourceIds, this, std::placeholders::_1)
    );
    this->_packetDispatcher.Register(
        Protocol::ServerToClient::ResourceRange,
        std::bind(&ResourceDownloader::_HandleResourceRange, this, std::placeholders::_1)
    );
}

void ResourceDownloader::_HandleNeededResourceIds(Common::Packet& p)
{
    PacketExtractor::ExtractNeededResourceIds(p, this->_nbNeededResources, this->_neededResourceIds);
    std::cout << "ResourceDownloader::_HandleNeededResourceIds: Getting "
              << this->_neededResourceIds.size() << " resources..." << std::endl;
    this->_CheckDownloadEnd();
}

void ResourceDownloader::_HandleResourceRange(Common::Packet& p)
{
    Uint32 id, offset, pluginId, totalSize, dataSize;
    std::string type, filename;
    char const* data = PacketExtractor::ExtractResourceRange(
        p, id, offset, pluginId, type, filename, totalSize, dataSize
    );

    if (id == 0)
        throw std::runtime_error("Wrong resource id");

    Common::Resource* res;
    if (!offset) // premier paquet pour cette resource
    {
        if (this->_inTransfer.find(id) != this->_inTransfer.end())
        {
            std::cerr << "ResourceDownloader::_HandleResourceRange: "
                      << "New resource packet for a resource already in transfer (" << id << " \"" << filename << "\")." << std::endl;
            return;
        }
        if (!totalSize)
        {
            std::cerr << "ResourceDownloader::_HandleResourceRange: Empty resource (" << id << " \"" << filename << "\")." << std::endl;
            ++this->_nbDownloadedResources;
            this->_CheckDownloadEnd();
            return;
        }
        res = new Common::Resource(id, pluginId, type, filename, 0, totalSize);
        this->_inTransfer[id] = res;
    }
    else // paquet data range seulement
    {
        if (this->_inTransfer.find(id) == this->_inTransfer.end())
        {
            std::cerr << "ResourceDownloader::_HandleResourceRange: "
                      << "Range resource packet for a resource not in transfer (" << id << " \"" << filename << "\")." << std::endl;
            return;
        }
        res = this->_inTransfer[id];
    }
    if (offset + dataSize > res->size)
    {
        std::cerr << "ResourceDownloader::_HandleResourceRange(): Resource range out of bounds (" << id << " \"" << filename << "\")." << std::endl;
        this->_inTransfer.erase(id);
        Tools::Delete(res);
        return; // le cache ne sera jamais fini d'etre chargé
    }
    res->WriteDataWithABadConstCast(data, dataSize, offset);
    if (offset + dataSize == res->size)
    {
        std::cout << "ResourceDownloader::_HandleResourceRange: Resource downloaded (" << id << " \"" << filename << "\" " << res->size << " bytes)." << std::endl;
        ++this->_nbDownloadedResources;
        this->_database.AddResource(*res);
        this->_inTransfer.erase(id);
        Tools::Delete(res);
        this->_CheckDownloadEnd();
    }
    else
    {
        auto p = PacketCreator::GetResourceRangePacket(res->id, offset + dataSize);
        this->_network.SendPacket(std::move(p));
    }
}

void ResourceDownloader::_CheckDownloadEnd()
{
    if (this->_nbDownloadedResources == this->_nbNeededResources)
    {
        std::cout << "ResourceDownloader::_HandleResourceRange: All needed resources downloaded." << std::endl;
        this->_database.ValidateUpdate();
        this->_eventManager.Notify<Tools::Gui::Event>("cache/loaded");
    }
    else
        this->_RequestNextResource();
}

void ResourceDownloader::_RequestNextResource()
{
    if (!this->_neededResourceIds.empty())
    {
        auto p = PacketCreator::GetResourceRangePacket(this->_neededResourceIds.front(), 0);
        this->_neededResourceIds.pop_front();
        this->_network.SendPacket(std::move(p));
    }
    //else
    //{
    //    this->_eventManager.Notify<Tools::Gui::Event>("cache/loaded");
    //}
}

