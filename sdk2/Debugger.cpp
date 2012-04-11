#include "sdk2/Debugger.hpp"
#include "sdk2/Sdk.hpp"
#include "sdk2/qt/MainWindow.hpp"
#include "sdk2/qt/ToolsSubWindow.hpp"
#include "sdk2/qt/ConnectSubWindow.hpp"

namespace Sdk {

    Debugger::Debugger(Sdk& sdk) :
        _sdk(sdk)
    {
        this->_toolsSubWindow = new Qt::ToolsSubWindow(this->_sdk);
        this->_connectSubWindow = new Qt::ConnectSubWindow(this->_sdk);
    }

    Debugger::~Debugger()
    {
        Tools::Delete(this->_connectSubWindow);
        Tools::Delete(this->_toolsSubWindow);
    }

}
