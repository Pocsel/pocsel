#include "sdk2/qt/ConnectSubWindow.hpp"

namespace Sdk { namespace Qt {

    ConnectSubWindow::ConnectSubWindow(Sdk& sdk) :
        _sdk(sdk)
    {
        this->setupUi(this);
    }

}}
