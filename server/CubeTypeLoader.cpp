#include <iostream>

#include "tools/lua/Interpreter.hpp"

#include "server/CubeTypeLoader.hpp"
#include "server/ResourceManager.hpp"

namespace Server {

    CubeTypeLoader::CubeTypeLoader(ResourceManager& resourceManager) :
        _resourceManager(resourceManager)
    {
    }

    bool CubeTypeLoader::Load(Common::CubeType& descr, std::string const& code)
    {
        Tools::Lua::Interpreter lua(code);
        try
        {
            auto textures = lua["textures"];
            descr.textures.top =    this->_resourceManager.GetId(textures["top"].as<std::string>());
            descr.textures.left =   this->_resourceManager.GetId(textures["left"].as<std::string>());
            descr.textures.front =  this->_resourceManager.GetId(textures["front"].as<std::string>());
            descr.textures.right =  this->_resourceManager.GetId(textures["right"].as<std::string>());
            descr.textures.back =   this->_resourceManager.GetId(textures["back"].as<std::string>());
            descr.textures.bottom = this->_resourceManager.GetId(textures["bottom"].as<std::string>());

            descr.solid = lua["solid"];
            descr.transparent = lua["transparent"];

            std::cout << descr.name << " " << descr.id << "\n";
            std::cout << descr.textures.top << "\n";
            std::cout << descr.textures.left << "\n";
            std::cout << descr.textures.front << "\n";
            std::cout << descr.textures.right << "\n";
            std::cout << descr.textures.back << "\n";
            std::cout << descr.textures.bottom << "\n";

            std::cout << descr.solid << "\n";
            std::cout << descr.transparent << "\n";

            std::cout << "\n";

            return true;
        }
        catch (std::exception& e)
        {
            std::cerr << "CubeTypeLoader::Load: " << e.what() << "." << std::endl;
            return false;
        }
    }

}
