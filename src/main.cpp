#include <QApplication>
#include <QDirIterator>

#include "Frontend.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    QDirIterator it(":", QDirIterator::Subdirectories);
    while (it.hasNext()) {
        qDebug() << it.next();
    }
    int n = 0;
    for(int i = 1; i < argc; i++){
        if((std::string)argv[i] == "-test"){
            n = 1;
            break;
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
