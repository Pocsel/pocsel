#ifndef __SDK_SDK_HPP__
#define __SDK_SDK_HPP__

#include <QApplication>

namespace Sdk {

    namespace Qt {
        class MainWindow;
    }

    class Sdk :
        private boost::noncopyable
    {
        private:
            QApplication _app;
            Qt::MainWindow* _mainWindow;

        public:
            Sdk(int& ac, char** av);
            int Run();
            Qt::MainWindow& GetMainWindow();
    };

}

#endif
