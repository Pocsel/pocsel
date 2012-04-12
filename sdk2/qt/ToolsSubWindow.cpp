#include "sdk2/qt/ToolsSubWindow.hpp"

namespace Sdk { namespace Qt {

    ToolsSubWindow::ToolsSubWindow(Sdk& sdk) :
        _sdk(sdk)
    {
        this->setupUi(this);
    }

}}
