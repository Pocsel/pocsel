#ifndef __SDK_QT_TOOLBOX_HPP__
#define __SDK_QT_TOOLBOX_HPP__

#include "sdk2/ui_ToolsSubWindow.h"

namespace Sdk {
    class Sdk;
}

namespace Sdk { namespace Qt {

    class ToolsSubWindow :
        public QToolBox, private ::Ui::ToolsSubWindow
    {
    Q_OBJECT

    private:
        Sdk& _sdk;

    public:
        ToolsSubWindow(Sdk& sdk);
    };

}}

#endif
