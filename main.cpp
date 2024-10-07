#include <QApplication>
#include "MainWindow.h"


int main(int argc, char* argv[])
{
    // istanzio applicazione Qt
    QApplication a(argc, argv);

    // lancia il gioco
    MainWindow::instance()->show();
    

    // eseguo applicazione Qt
    return a.exec();
}