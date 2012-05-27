#ifndef __TOOLS_MODELS_ERRORMODEL_HPP__
#define __TOOLS_MODELS_ERRORMODEL_HPP__

#include "tools/models/MqmModel.hpp"

namespace Tools { namespace Models {

    std::unique_ptr<MqmModel> ErrorModel(/*MqmModel::TextureCallback textureCallback, */Tools::IRenderer& renderer);

}}

#endif
