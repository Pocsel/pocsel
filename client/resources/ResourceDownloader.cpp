#include "client/precompiled.hpp"

#include "client/network/Network.hpp"
#include "client/network/PacketCreator.hpp"
#include "client/network/PacketExtractor.hpp"
#include "client/resources/CacheDatabaseProxy.hpp"
#include "client/resources/ResourceDownloader.hpp"

#include "common/Resource.hpp"

namespace Client { namespace Resources {

    ResourceDownloader::ResourceDownloader(CacheDatabaseProxy& database, Network::Network& network)
        : _database(database),
        _network(network),
        _nbNeededResources(0),
        _nbDownloadedResources(0),
        _loading(0)
    {
    }

    void ResourceDownloader::AskResources(Uint32 nbResources, std::list<Uint32>& neededResources)
    {
        this->_nbNeededResources = nbResources;
        this->_neededResourceIds.splice(this->_neededResourceIds.end(), neededResources);
        this->_CheckDownloadEnd();
    }

    void ResourceDownloader::HandleResourceRange(Common::Packet& p)
    {
        Uint32 id, offset, pluginId, totalSize, dataSize;
        std::string type, filename;
        char const* data = Network::PacketExtractor::ExtractResourceRange(p, id, offset, pluginId, type, filename, totalSize, dataSize);

        if (id == 0)
            throw std::runtime_error("Wrong resource id");

        Common::Resource* res;
        if (!offset) // premier paquet pour cette resource
        {
            if (this->_inTransfer.find(id) != this->_inTransfer.end())
            {
                Tools::error << "New resource packet for a resource already in transfer (" << id << " \"" << filename << "\").\n";
                return;
            }
            if (!totalSize)
            {
                Tools::error << "Empty resource (" << id << " \"" << filename << "\").\n";
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
                Tools::error << "Range resource packet for a resource not in transfer (" << id << " \"" << filename << "\").\n";
                return;
            }
            res = this->_inTransfer[id];
        }
        if (offset + dataSize > res->size)
        {
            Tools::error << "Resource range out of bounds (" << id << " \"" << filename << "\").\n";
            this->_inTransfer.erase(id);
            Tools::Delete(res);
            return; // le cache ne sera jamais fini d'etre chargé
        }
        res->WriteDataWithABadConstCast(data, dataSize, offset);
        if (offset + dataSize == res->size)
        {
            Tools::log << "Resource downloaded (" << id << " \"" << filename << "\" " << res->size << " bytes).\n";
            ++this->_nbDownloadedResources;
            this->_database.AddResource(*res);
            this->_inTransfer.erase(id);
            Tools::Delete(res);
            this->_CheckDownloadEnd();
        }
        else
            this->_network.SendPacket(Network::PacketCreator::GetResourceRange(res->id, offset + dataSize));
    }

    void ResourceDownloader::_CheckDownloadEnd()
    {
        if (this->_nbDownloadedResources == this->_nbNeededResources)
        {
            this->_loading = 1.0f;
            Tools::log << "All needed resources downloaded.\n";
            this->_database.ValidateUpdate();
        }
        else
            this->_RequestNextResource();
    }

    void ResourceDownloader::_RequestNextResource()
    {
        if (!this->_neededResourceIds.empty())
        {
            this->_loading = this->_nbDownloadedResources / (float)this->_nbNeededResources;
            this->_network.SendPacket(Network::PacketCreator::GetResourceRange(this->_neededResourceIds.front(), 0));
            this->_neededResourceIds.pop_front();
        }
    }

}}
