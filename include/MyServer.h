#ifndef RTCHESS2_MYSERVER_H
#define RTCHESS2_MYSERVER_H

#include <QWidget>
#include <QObject>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
#include "ChessManager.h"

class MyServer: public QWidget {
    Q_OBJECT
public:
    explicit MyServer(bool isHost, int port);
    //Создание экземпляра MyServer с QTcpServer, QTcpSocket
    ~MyServer(){
        delete ServerHost;
        delete thisSocket;
        delete thisChessManager;
    }

    void StartGame();
    //создание экземпляра ChessManager, когда подключение прошло и самое время играть

    void MakeMove(std::pair<int,int>& pos, std::pair<int,int>& target);
    //выполнение хода
    //если хост, то сразу ход и отправляет гостю
    //если гость, то сначала отправляет хосту, а затем либо получает сообщение от хоста и делает ход, либо не получает

    QTcpServer* ServerHost;                                 //Сервер хоста
    QTcpSocket* thisSocket;                                //Сокет гостя, или целевой сокет для сервера

    ChessManager* thisChessManager;

    void SendData(QTcpSocket *targetSocket, const std::pair<int, int> dataPairPos, const std::pair<int, int> dataPairTarget);
    //отправка другому сокету хода вида {currentPositionY, currentPositionX} {targetPositionY, targetPositionX}
    //вызывается по необходимости(новый ход, или потверждение хода гостя)
    void ReadData();
    //получение хода вида {currentPositionY, currentPositionX} {targetPositionY, targetPositionX}
    //вызывается при появлинии новых данных

    bool SearchInDirection(std::pair<int,int>& currentPos, std::pair<int,int>& target);
private:
    bool isHost = false;
    //перенос из данного MainWindow
    qint16 blockSize = 0;
    //размер блока, передаваемого другому игроку

    bool movePossible(std::pair<int,int>& currentPos, std::pair<int,int>& target);
    //проверка хода на легальность:
    //1) Существует ли фигура, которой ходят;
    //2) Есть ли на пути вражеские фигуры;
    std::vector<std::pair<int,int>> receivedPackage;
    std::vector<std::pair<int,int>> sendedPackage;

    std::vector<std::pair<int,int>> ConvertToPair(const QVector<int> data);
    QVector<int> ConvertToQVector(const std::vector<std::pair<int,int>> data);
    std::pair<int,int> RemakePair(const std::pair<int,int> data){
        return {7 - data.first,7 - data.second};
    }
    //инвертирование хода для вражеской доски
};


#endif
