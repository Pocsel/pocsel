#ifndef __SDK_QT_CONNECTSUBWINDOW_HPP__
#define __SDK_QT_CONNECTSUBWINDOW_HPP__

#include "sdk2/ui_ConnectSubWindow.h"

namespace Sdk {
    class Sdk;
}

namespace Sdk { namespace Qt {

    class ConnectSubWindow :
        public QWidget, private ::Ui::ConnectSubWindow
    {
    Q_OBJECT

    private:
        Sdk& _sdk;

    public:
        ConnectSubWindow(Sdk& sdk);
    };

}}

#endif
