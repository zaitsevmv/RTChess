#include "timemanager.h"

void TimeManager::FixedUpdateCoolDown() {
    timer++;
    std::vector<std::pair<int,int>> tempTimePairs;
    for(auto& i: PositionsTime){
        if(AllTimes[i.first][i.second] != 0){
            tempTimePairs.push_back(i);
            AllTimes[i.first][i.second]--;
        }
    }
    PositionsTime = tempTimePairs;
}

TimeManager::~TimeManager() {
    thisTimer = nullptr;
    delete(thisTimer);
}

TimeManager::TimeManager()
        :timer(-5) {
    thisTimer = new QTimer();
    thisTimer->start(1000);
}
