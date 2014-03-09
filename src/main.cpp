#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    a.setOrganizationName("tomleese.me.uk");
    a.setApplicationName("tvnamer");
    
    MainWindow w;
    w.show();
    
    return a.exec();
}
