#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    char ARG_DISABLE_WEB_SECURITY[] = "--disable-web-security";
    int newArgc = argc + 1 + 1;
    char** newArgv = new char*[newArgc];
    for(auto i = 0; i < argc; ++i) {
        newArgv[i] = argv[i];
    }
    newArgv[argc] = ARG_DISABLE_WEB_SECURITY;
    newArgv[argc + 1] = nullptr;

    QApplication a(newArgc, newArgv);
    MainWindow w;
    w.setWindowFlag(Qt::WindowStaysOnTopHint);
    w.show();
    return a.exec();
}
