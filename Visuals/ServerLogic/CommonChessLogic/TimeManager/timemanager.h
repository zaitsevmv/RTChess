#ifndef RTCHESS2_TIMEMANAGER_H
#define RTCHESS2_TIMEMANAGER_H

#include <vector>
#include <QWidget>
#include <QTimer>
#include <QDebug>

class TimeManager: public QWidget {
Q_OBJECT
public:
    TimeManager();
    ~TimeManager();
    int timer = -5;
    std::vector<std::vector<int>> AllTimes =
            std::vector<std::vector<int>>(8,std::vector<int>(8,0));
    //все перезарядки на поле 8 на 8
    std::vector<std::pair<int,int>> PositionsTime;
    //все поля, на которых перезарядка
    QTimer* thisTimer;
    //таймер, отсчитывающий по 1 секунде

    void FixedUpdateCoolDown();
    //вызывается каждую секунду, перезаряжает фигуры, обновляет таймеры
};


#endif //RTCHESS2_TIMEMANAGER_H
