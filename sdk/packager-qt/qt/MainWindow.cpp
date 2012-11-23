#include <QFileDialog>
#include <QMessageBox>

#include "sdk/packager-qt/qt/MainWindow.hpp"
#include "sdk/packager-qt/Packager.hpp"
#include "common/constants.hpp"
#include "tools/plugin-create/Create.hpp"

namespace Sdk { namespace PackagerQt { namespace Qt {

    MainWindow::MainWindow(Packager& packager) :
        _packager(packager), _replaceWithoutAsking(false)
    {
        this->setupUi(this);
        Tools::log.RegisterCallback(std::bind(&MainWindow::_Log, this, std::placeholders::_1));
    }

    void MainWindow::on_pluginRootPushButton_clicked()
    {
        QString pluginRoot = QFileDialog::getExistingDirectory(this, "Select plugin root directory");
        if (pluginRoot.size())
            this->pluginRootLineEdit->setText(pluginRoot);
    }

    void MainWindow::on_destFilePushButton_clicked()
    {
        QString destFile = QFileDialog::getSaveFileName(this, "Select plugin destination file", "", "Pocsel plugin (*." + QString(Common::PluginFileExt) + ")", 0, QFileDialog::DontConfirmOverwrite);
        if (destFile.size())
            this->destFileLineEdit->setText(destFile);
    }

    void MainWindow::on_packagePushButton_clicked()
    {
        if (!_replaceWithoutAsking &&
                QFile::exists(this->destFileLineEdit->text()) &&
                QMessageBox::question(this, "Replace", "The destination file already exists. Do you want to replace it?", QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::No)
            return;
        this->_Log("--------------------------------\n");
        Tools::PluginCreate::Create(boost::filesystem::path(this->pluginRootLineEdit->text().toStdString()), boost::filesystem::path(this->destFileLineEdit->text().toStdString()));
    }

    void MainWindow::on_replaceCheckBox_stateChanged(int state)
    {
        _replaceWithoutAsking = (state == ::Qt::Checked);
    }

    void MainWindow::_Log(std::string const& message)
    {
        // foireux
        QTextCursor cursor = this->logPlainTextEdit->textCursor();
        cursor.movePosition(QTextCursor::End);
        this->logPlainTextEdit->setTextCursor(cursor);
        this->logPlainTextEdit->insertPlainText(QString::fromStdString(message));

        // foireux aussi
        this->_packager.GetApplication().processEvents(QEventLoop::ExcludeUserInputEvents);
    }

}}}
