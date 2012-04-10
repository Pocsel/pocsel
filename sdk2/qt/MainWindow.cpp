#include "sdk2/qt/MainWindow.hpp"
#include "sdk2/ProgramInfo.hpp"

namespace Sdk { namespace Qt {

    MainWindow::MainWindow(Sdk& sdk) :
        _sdk(sdk)
    {
        this->setupUi(this);
        this->versionLabel->setText(PROJECT_NAME " " PROGRAM_NAME " version " GIT_VERSION);
    }

}}
