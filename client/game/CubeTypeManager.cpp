#include "client/precompiled.hpp"

#include "client/Client.hpp"
#include "client/game/CubeType.hpp"
#include "client/game/CubeTypeManager.hpp"
#include "client/network/PacketCreator.hpp"

#include "common/Packet.hpp"
#include "common/FieldValidator.hpp"
#include "tools/lua/Interpreter.hpp"

namespace Client { namespace Game {

    CubeTypeManager::CubeTypeManager(Client& client, Common::BaseChunk::CubeType nbCubeTypes)
        : _nbCubeTypes(nbCubeTypes),
        _client(client)
    {
        Tools::debug << "CubeTypeManager: Number of cube types: " << this->_nbCubeTypes << std::endl;
        this->_curAskedType = 1;
        this->_nbCubeTypes = nbCubeTypes;
        this->_AskOneType();
    }

    void CubeTypeManager::AddCubeType(std::unique_ptr<Common::CubeType> type)
    {
        Tools::debug << "CubeType: " << type->name << " (id: " << type->id << ", material: " << type->material << ")\n";
        this->_cubeTypes.push_back(CubeType(*type));
        this->_AskOneType();
    }

    void CubeTypeManager::LoadMaterials()
    {
        auto it = this->_cubeTypes.begin();
        auto itEnd = this->_cubeTypes.end();
        for (; it != itEnd; ++it)
        {
            auto material = this->_materials[it->pluginId].find(it->material);
            if (material != this->_materials[it->pluginId].end())
                it->LoadMaterial(this->_client.GetGame().GetResourceManager(), material->second);
            else
                throw std::runtime_error("Cube material \"" + it->material + "\" not found for cube type \"" + it->name + "\" (plugin: " + Tools::ToString(it->pluginId) + ").");
        }
    }

    void CubeTypeManager::_AskOneType()
    {
        assert(this->_nbCubeTypes != 0);
        if (this->_curAskedType <= this->_nbCubeTypes)
            this->_client.GetNetwork().SendPacket(Network::PacketCreator::GetCubeType(this->_curAskedType++));
    }

    void CubeTypeManager::RegisterLuaFunctions()
    {
        auto cubeMaterialNs = this->_client.GetGame().GetInterpreter().Globals().GetTable("Client").GetTable("CubeMaterial");
        cubeMaterialNs.Set("Register", this->_client.GetGame().GetInterpreter().MakeFunction(
                    std::bind(&CubeTypeManager::_ApiRegister, this, std::placeholders::_1)));
    }

    void CubeTypeManager::_ApiRegister(Tools::Lua::CallHelper& helper)
    {
        Uint32 pluginId = this->_client.GetGame().GetEngine().GetRunningPluginId();
        if (!pluginId)
            throw std::runtime_error("Client.CubeMaterial.Register: Could not determine currently running plugin, aborting registration");
        std::string cubeMaterialName;
        Tools::Lua::Ref prototype(helper.PopArg("Client.CubeMaterial.Register: Missing argument \"prototype\""));
        if (!prototype.IsTable())
            throw std::runtime_error("Client.CubeMaterial.Register: Argument \"prototype\" must be of type table (instead of " + prototype.GetTypeName() + ")");
        if (!prototype["cubeMaterialName"].IsString())
            throw std::runtime_error("Client.CubeMaterial.Register: Field \"cubeMaterialName\" must exist and be of type string");
        if (!Common::FieldValidator::IsRegistrableType(cubeMaterialName = prototype["cubeMaterialName"].ToString()))
            throw std::runtime_error("Client.CubeMaterial.Register: Invalid cube material name \"" + cubeMaterialName + "\"");
        if (this->_materials[pluginId].count(cubeMaterialName))
            throw std::runtime_error("Client.CubeMaterial.Register: A cube material with the name \"" + cubeMaterialName + "\" already exists in this plugin (" + Tools::ToString(pluginId) + ")");
        this->_materials[pluginId].insert(std::make_pair(cubeMaterialName, prototype));
        Tools::debug << "Cube material \"" << cubeMaterialName << "\" registered (plugin: " << pluginId << ")." << std::endl;
    }

}}
