#include <QFileDialog>

#include "packager-qt/qt/MainWindow.hpp"
#include "common/constants.hpp"

namespace PackagerQt { namespace Qt {

    MainWindow::MainWindow(Packager& packager) :
        _packager(packager)
    {
        this->setupUi(this);
    }

    void MainWindow::on_pluginRootPushButton_clicked()
    {
        this->pluginRootLineEdit->setText(QFileDialog::getExistingDirectory(this, "Select plugin root directory"));
    }

    void MainWindow::on_destFilePushButton_clicked()
    {
        this->destFileLineEdit->setText(QFileDialog::getOpenFileName(this, "Select plugin destination file", "", "Pocsel plugin (*." + QString(Common::PluginFileExt) + ")"));
    }

}}
