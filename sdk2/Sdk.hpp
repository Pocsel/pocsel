#ifndef __SDK_SDK_HPP__
#define __SDK_SDK_HPP__

#include <QApplication>

namespace Sdk {

    namespace Qt {
        class MainWindow;
    }
    class Debugger;

    class Sdk :
        private boost::noncopyable
    {
    private:
        QApplication _app;
        Qt::MainWindow* _mainWindow;
        Debugger* _debugger;

    public:
        Sdk(int& ac, char** av);
        ~Sdk();
        int Run();
        Debugger& GetDebugger();
        Qt::MainWindow& GetMainWindow();
    };

}

#endif
