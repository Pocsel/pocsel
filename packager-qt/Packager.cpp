#include "packager-qt/Packager.hpp"
#include "packager-qt/qt/MainWindow.hpp"

namespace PackagerQt
{

    Packager::Packager(int& ac, char** av) :
        _app(ac, av)
    {
        this->_mainWindow = new Qt::MainWindow(*this);
        this->_mainWindow->show();
    }

    Packager::~Packager()
    {
        Tools::Delete(this->_mainWindow);
    }

    int Packager::Run()
    {
        return this->_app.exec();
    }

    Qt::MainWindow& Packager::GetMainWindow()
    {
        return *this->_mainWindow;
    }

}
