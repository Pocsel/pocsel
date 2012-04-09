#include "sdk2/qt/MainWindow.hpp"

namespace Sdk { namespace Qt {

    MainWindow::MainWindow(Sdk& sdk) :
        _sdk(sdk)
    {
        this->setupUi(this);
    }

}}
