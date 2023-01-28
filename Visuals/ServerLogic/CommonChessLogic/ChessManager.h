#ifndef RTCHESS_CHESSMANAGER_H
#define RTCHESS_CHESSMANAGER_H

#include <vector>

#include "TimeManager/timemanager.h"

class ChessManager: public QObject{
    Q_OBJECT
public:
    ChessManager() {  }
    ~ChessManager() {
        delete thisTimeManager;
    }
    ChessManager(const ChessManager& root) = delete;

    std::vector<std::pair<int,int>> FindPossibleMoves(int& posY, int& posX);
    //Поиск всех возможных ходов для вигуры на данном поле
    //поиск идет по правилам шахмат, рокировка запрещена
    bool pieceExists(std::pair<int,int>& pos);
    //проверка на возможность у данной фигуры сделать ход
    bool moveFair(int& posY, int& posX, std::vector<std::pair<int,int>>& possibleMoves);
    //проверка хода на легальность
    void GenerateStartPosition(int& colour);
    //Создает начальную позицию
    //не зависит от цвета, хоста и тп
    void MakeMove(std::pair<int,int>& pos, std::pair<int,int>& target);
    void CommitMove(std::pair<int,int> pos, std::pair<int,int> target);
    //Выполняет ход
    //может вызваться только данным MyServer после всех проверок
    //будет выполнен при любых условиях, даже если по правилам шахмат это невозможно
    //вызывает DoAllTheJobAfterMove у данного MainWindow
    TimeManager *thisTimeManager = new TimeManager;
    //создает объект контроля за перезарядками

    std::vector<std::vector<int>> AllPositions =
            std::vector<std::vector<int>>(8,std::vector<int>(8,0));
    //все позиции
    //таблица 8 на 8, где >0 - союзные фигуры (1 - пешка, 2 - слон, 3 - конь, 4 - ладья, 5 - ферзь, 6 - король)
    //<0 аналогичные вражеские фигуры

    int oneTeamPieces = 16;
    int otherTeamPieces = 16;
    //счетчики фигур
    //когда 0, то вызывается экран завершения игры
signals:
    void MoveCommited(std::pair<int,int>&, std::pair<int,int>&, bool);
    //вызов DoAllTheJobAfterMove
    void GameEndedWin();
    void GameEndedLose();
    //вызов экрана завершения игры
private:
    int cdMove = 5;
    //базовая перезарядка хода в секундах
    std::vector<std::pair<int,int>> EnPassant;
    //механика взятия на проходе
    int GetId(std::pair<int,int> pos);
    //возвращает id данной клетки
    void SearchDiagonally(bool& directionPossible, int debthY, int debthX, int& posY, int& posX,
                            std::vector<std::pair<int,int>>& outputPossibleMoves);
    //поиск возможного ходоа с параметром глубины по Y, и по X
};

#endif //RTCHESS_CHESSMANAGER_H
