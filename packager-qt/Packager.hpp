#ifndef __PACKAGER_QT_PACKAGER_HPP__
#define __PACKAGER_QT_PACKAGER_HPP__

#include <QApplication>

namespace PackagerQt {

    namespace Qt {
        class MainWindow;
    }

    class Packager :
        private boost::noncopyable
    {
    private:
        QApplication _app;
        Qt::MainWindow* _mainWindow;

    public:
        Packager(int& ac, char** av);
        ~Packager();
        int Run();
        Qt::MainWindow& GetMainWindow();
    };

}

#endif
