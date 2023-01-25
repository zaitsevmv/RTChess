#include <QApplication>

#include "Visuals/Frontend.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    int n = 0;
    if(argc > 1){
        if((std::string)argv[1] == "-test"){
            n = 1;
        }
    }
    MainWindow w;
    if(n != 0){
        w.ForTests = true;
        w.testWidget->show();
    }
    w.RunGameMenu();
    w.show();
    if(n != 0){
        w.testWidget->show();
    }
    return QApplication::exec();
}
