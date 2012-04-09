#include "sdk2/Sdk.hpp"
#include "sdk2/qt/MainWindow.hpp"

namespace Sdk
{

    Sdk::Sdk(int& ac, char** av) :
        _app(ac, av)
    {
        this->_mainWindow = new Qt::MainWindow(*this);
        this->_mainWindow->show();
    }

    int Sdk::Run()
    {
        return this->_app.exec();
    }

    Qt::MainWindow& Sdk::GetMainWindow()
    {
        return *this->_mainWindow;
    }

}
