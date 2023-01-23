#include <QApplication>

#include "Visuals/Frontend.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    MainWindow w;
    w.RunGameMenu();
    w.show();

    //MainWindow w;
    //w.ForTests = true;
    //w.RunGameMenu();
    //w.show();
    //w.testWidget->show();
    //пример приложения для тестов

    //MainWindow w1;
    //w1.RunGameMenu();
    //w1.show();
    //пример стандартного приложения
    return QApplication::exec();
}
