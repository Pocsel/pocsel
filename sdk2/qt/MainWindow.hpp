#ifndef __SDK_QT_MAINWINDOW_HPP__
#define __SDK_QT_MAINWINDOW_HPP__

#include "sdk2/ui_main.h"

namespace Sdk {
    class Sdk;
}

namespace Sdk { namespace Qt {

    class MainWindow :
        public QMainWindow, private ::Ui::MainWindow
    {
    Q_OBJECT

    private:
        Sdk& _sdk;

    public:
        MainWindow(Sdk& sdk);
    };

}}

#endif
