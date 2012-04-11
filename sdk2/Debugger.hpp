#ifndef __SDK_DEBUGGER_HPP__
#define __SDK_DEBUGGER_HPP__

namespace Sdk {

    namespace Qt {
        class ToolsSubWindow;
        class ConnectSubWindow;
    }
    class Sdk;

    class Debugger
    {
    private:
        Sdk& _sdk;
        Qt::ToolsSubWindow* _toolsSubWindow;
        Qt::ConnectSubWindow* _connectSubWindow;

    public:
        Debugger(Sdk& sdk);
        ~Debugger();
    };

}

#endif
