#ifndef __SERVER_CUBETYPE_LOADER__
#define __SERVER_CUBETYPE_LOADER__

#include <string>

#include "common/CubeType.hpp"

namespace Server {

    class ResourceManager;

    class CubeTypeLoader
    {
        private:
            ResourceManager& _resourceManager;

        public:
            CubeTypeLoader(ResourceManager& resourceManager);
            bool Load(Common::CubeType& descr, std::string const& code);
        private:
            Uint32 _GetResourceId(std::string const& varname);
    };

}

#endif
