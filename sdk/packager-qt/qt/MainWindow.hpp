#ifndef __SDK_PACKAGER_QT_QT_MAINWINDOW_HPP__
#define __SDK_PACKAGER_QT_QT_MAINWINDOW_HPP__

#include "sdk/packager-qt/ui_MainWindow.h"

namespace Sdk { namespace PackagerQt {
    class Packager;
}}

namespace Sdk { namespace PackagerQt { namespace Qt {

    class MainWindow :
        public QMainWindow, private ::Ui::MainWindow
    {
    Q_OBJECT

    private:
        Packager& _packager;
        bool _replaceWithoutAsking;
    private slots:
        void on_pluginRootPushButton_clicked();
        void on_destFilePushButton_clicked();
        void on_packagePushButton_clicked();
        void on_replaceCheckBox_stateChanged(int state);

    public:
        MainWindow(Packager& packager);
    private:
        void _Log(std::string const& message);
    };

}}}

#endif
