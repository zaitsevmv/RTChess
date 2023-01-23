#include "MyServer.h"
#include <chrono>
#include <thread>
#include <algorithm>
#include <vector>

MyServer::MyServer(const bool isHost, int port)
    :isHost(isHost) {
    thisChessManager = new ChessManager;
    if(isHost){
        ServerHost = new QTcpServer;
        ServerHost->listen(QHostAddress::LocalHost,33333+port);
    } else{
        thisSocket = new QTcpSocket;
        thisSocket->open(QIODevice::ReadWrite);
    }
}

bool MyServer::movePossible(std::pair<int, int> &currentPos, std::pair<int, int> &target) {
    //проверка хода на легальность:
    //1) Существует ли фигура, которой ходят;
    //2) Есть ли на пути вражеские фигуры;
    int posY = currentPos.first;
    int posX = currentPos.second;
    if(thisChessManager->AllPositions[posY][posX] <= 0){ //есть ли там союзная фигура?
        return false;
    }
    int tag = std::abs(thisChessManager->AllPositions[posY][posX]);
    if(tag != 1 && tag != 3 && tag != 6){
        return SearchInDirection(currentPos, target);
    }
    return true;
}

void MyServer::ReadData() {
    //получение хода вида {currentPositionY, currentPositionX} {targetPositionY, targetPositionX}
    //вызывается при появлинии новых данных
    QVector<int> data;
    QDataStream in(thisSocket);
    in.setVersion(QDataStream::Qt_5_0);

    for(;;){
        if(blockSize == 0){
            if(thisSocket->bytesAvailable() < sizeof(quint16)){
                break;
            }
            in >> blockSize;
        }
        if(thisSocket->bytesAvailable() < blockSize){
            break;
        }

        in >> data;
        receivedPackage = ConvertToPair(data);
        blockSize = 0;

        if(isHost) {// && movePossible(receivedPackage[0],receivedPackage[1])){
            SendData(thisSocket,RemakePair(receivedPackage[0]),RemakePair(receivedPackage[1]));
            thisChessManager->CommitMove(receivedPackage[0],receivedPackage[1]);
        }
        if(!isHost){
            thisChessManager->CommitMove(receivedPackage[0],receivedPackage[1]);
        }
    }
}

bool MyServer::SearchInDirection(std::pair<int, int> &currentPos, std::pair<int, int> &target)
{
    int debthY = target.first - currentPos.first, debthX = target.second - currentPos.second;
    int goY = 0, goX = 0;
    if(debthX != 0){
        goX = debthX/std::abs(debthX);
    }
    if(debthY != 0){
        goY = debthY/std::abs(debthY);
    }
    int pY = goX, pX = goY;
    while(currentPos.first + pY != target.first - goY && currentPos.second + pX != target.second - goX){
        if(thisChessManager->AllPositions[currentPos.first + pY][currentPos.second + pX] != 0){
            return false;
        }
    }
    return true;
}

void MyServer::SendData(QTcpSocket *targetSocket, const std::pair<int, int> dataPairPos, const std::pair<int, int> dataPairTarget) {
    //отправка другому сокету хода вида {currentPositionY, currentPositionX} {targetPositionY, targetPositionX}
    //вызывается по необходимости
    QVector<int> data = ConvertToQVector({dataPairPos, dataPairTarget});
    QByteArray arrBlock;
    arrBlock.clear();
    QDataStream out(&arrBlock,QIODevice::ReadWrite);
    out.setVersion(QDataStream::Qt_5_0);

    out << qint16(0) << data;
    out.device()->seek(0);
    out << qint16(arrBlock.size() - sizeof(qint16));

    thisSocket->write(arrBlock);
}

std::vector<std::pair<int, int>> MyServer::ConvertToPair(const QVector<int> data) {
    return std::vector<std::pair<int, int>>{{data[0],data[1]},
                                            {data[2], data[3]}};
}


QVector<int> MyServer::ConvertToQVector(const std::vector<std::pair<int, int> > data) {
    return QVector<int>{data[0].first, data[0].second, data[1].first, data[1].second};
}

void MyServer::StartGame() {
    //создание шахматной доски, в которой описывается вся логика, и таймер
    thisChessManager = new ChessManager;
}

void MyServer::MakeMove(std::pair<int, int> &pos, std::pair<int, int> &target) {
    //targetSocket->open(QIODevice::ReadOnly);
    if(isHost){
        //если хост, то сразу ход и отправляет гостю
        //если гость, то сначала отправляет хосту, а затем либо получает сообщение от хоста и делает ход, либо не получает
        thisChessManager->CommitMove(pos,target);
    }
    SendData(thisSocket, RemakePair(pos), RemakePair(target));
}
