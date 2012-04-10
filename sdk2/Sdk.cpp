#include "sdk2/Sdk.hpp"
#include "sdk2/Debugger.hpp"
#include "sdk2/qt/MainWindow.hpp"

namespace Sdk
{

    Sdk::Sdk(int& ac, char** av) :
        _app(ac, av)
    {
        this->_mainWindow = new Qt::MainWindow(*this);
        this->_mainWindow->show();
        this->_debugger = new Debugger(*this);
    }

    Sdk::~Sdk()
    {
        Tools::Delete(this->_debugger);
        Tools::Delete(this->_mainWindow);
    }

    int Sdk::Run()
    {
        return this->_app.exec();
    }

    Debugger& Sdk::GetDebugger()
    {
        return *this->_debugger;
    }

    Qt::MainWindow& Sdk::GetMainWindow()
    {
        return *this->_mainWindow;
    }

}
