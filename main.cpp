#include "widget.h"

#include <QThread>
#include <QApplication>
#include "cmdevice.h"
#include "cmprotocolhandler.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    w.show();
    return a.exec();
}
