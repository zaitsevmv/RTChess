#include <QDebug>
#include "ChessManager.h"

int ChessManager::GetId(std::pair<int,int> pos){
    return AllPositions[pos.first][pos.second];
}

void ChessManager::SearchDiagonally(bool& directionPossible, int debthY, int debthX, int& posY, int& posX,
                                      std::vector<std::pair<int,int>>& outputPossibleMoves){
    if(directionPossible){
        if(posY+debthY > 7 || posX+debthX > 7 || posY+debthY < 0 || posX+debthX < 0){
            directionPossible = false;
            return;
        } else{
            int newPosX = posX+debthX;
            int newPosY = posY+debthY;
            if(GetId({newPosY,newPosX}) == 0){
                outputPossibleMoves.emplace_back(newPosY,newPosX);
            } else if(GetId({newPosY,newPosX}) < 0){
                outputPossibleMoves.emplace_back(newPosY,newPosX);
                directionPossible = false;
            } else{
                directionPossible = false;
            }
        }
    }
}

std::vector<std::pair<int,int>> ChessManager::FindPossibleMoves(int &posY, int &posX) {
    std::vector<std::pair<int,int>> outputPossibleMoves;
    switch (GetId({posY, posX})) {
        case 1:  //pawn
        {
            int maxRange = 1;
            if(posY == 0){
                break;
            }
            std::pair<int,int> leftMove = {posY-1,posX-1};
            std::pair<int,int> rightMove = {posY-1,posX+1};

            if(posX > 0 && GetId(leftMove) < 0){
                outputPossibleMoves.push_back(leftMove);
            }
            if(posX < 7 && GetId(rightMove) < 0){
                outputPossibleMoves.push_back(rightMove);
            }
            for(auto& enemyPawnsMoves: EnPassant){
                if(leftMove == enemyPawnsMoves || rightMove == enemyPawnsMoves){
                    outputPossibleMoves.push_back(enemyPawnsMoves);
                }
            }
            if(posY == 6){
                maxRange++;
            }
            for(int i = posY-1; i >= posY-maxRange; i--){
                int nextCell = AllPositions[i][posX];
                if(nextCell == 0){
                    outputPossibleMoves.emplace_back(i, posX);
                }
            }
            break;
        }
        case 2: //bishop
        {
            bool leftDown = true, rightDown = true, leftUp = true, rightUp = true;
            int debth = 1;

            while(leftDown || rightDown || leftUp || rightUp){
                SearchDiagonally(leftDown,debth,-debth,posY,posX,outputPossibleMoves);
                SearchDiagonally(rightDown,debth,debth,posY,posX,outputPossibleMoves);
                SearchDiagonally(leftUp,-debth,-debth,posY,posX,outputPossibleMoves);
                SearchDiagonally(rightUp,-debth,debth,posY,posX,outputPossibleMoves);
                debth++;
            }
            break;
        }
        case 3: //Knight
        {
            bool knightMove = true;
            int debth = 1;

            SearchDiagonally(knightMove, 2 * debth, debth, posY, posX, outputPossibleMoves);
            knightMove = true;
            SearchDiagonally(knightMove, 2 * debth, -debth, posY, posX, outputPossibleMoves);
            knightMove = true;
            SearchDiagonally(knightMove, -2 * debth, debth, posY, posX, outputPossibleMoves);
            knightMove = true;
            SearchDiagonally(knightMove, -2 * debth, -debth, posY, posX, outputPossibleMoves);
            knightMove = true;
            SearchDiagonally(knightMove, debth, 2 * debth, posY, posX, outputPossibleMoves);
            knightMove = true;
            SearchDiagonally(knightMove, -debth, 2 * debth, posY, posX, outputPossibleMoves);
            knightMove = true;
            SearchDiagonally(knightMove, debth, -2 * debth, posY, posX, outputPossibleMoves);
            knightMove = true;
            SearchDiagonally(knightMove, -debth, -2 * debth, posY, posX, outputPossibleMoves);
            break;
        }
        case 4: //Rook
            {
                bool left = true , right = true, up = true, down = true;
                int debth = 1;

                while(left || right || up || down){
                    SearchDiagonally(left,0,-debth,posY,posX,outputPossibleMoves);
                    SearchDiagonally(right,0,debth,posY,posX,outputPossibleMoves);
                    SearchDiagonally(up,-debth,0,posY,posX,outputPossibleMoves);
                    SearchDiagonally(down,debth,0,posY,posX,outputPossibleMoves);
                    debth++;
                }
                break;
            }
        case 5: //Queen
        {
            bool left = true , right = true, up = true, down = true;
            bool leftDown = true, rightDown = true, leftUp = true, rightUp = true;
            int debth = 1;

            while(left || right || up || down){
                SearchDiagonally(left,0,-debth,posY,posX,outputPossibleMoves);
                SearchDiagonally(right,0,debth,posY,posX,outputPossibleMoves);
                SearchDiagonally(up,-debth,0,posY,posX,outputPossibleMoves);
                SearchDiagonally(down,debth,0,posY,posX,outputPossibleMoves);
                debth++;
            }
            debth = 1;

            while(leftDown || rightDown || leftUp || rightUp){
                SearchDiagonally(leftDown,debth,-debth,posY,posX,outputPossibleMoves);
                SearchDiagonally(rightDown,debth,debth,posY,posX,outputPossibleMoves);
                SearchDiagonally(leftUp,-debth,-debth,posY,posX,outputPossibleMoves);
                SearchDiagonally(rightUp,-debth,debth,posY,posX,outputPossibleMoves);
                debth++;
            }
            break;
        }
        case 6:
        {
            bool kingMove = true;
            int debth = 1;

            SearchDiagonally(kingMove,-debth,0,posY,posX,outputPossibleMoves);
            kingMove = true;
            SearchDiagonally(kingMove,debth,0,posY,posX,outputPossibleMoves);
            kingMove = true;
            SearchDiagonally(kingMove,0,-debth,posY,posX,outputPossibleMoves);
            kingMove = true;
            SearchDiagonally(kingMove,0,debth,posY,posX,outputPossibleMoves);
            kingMove = true;
            SearchDiagonally(kingMove,-debth,debth,posY,posX,outputPossibleMoves);
            kingMove = true;
            SearchDiagonally(kingMove,debth,debth,posY,posX,outputPossibleMoves);
            kingMove = true;
            SearchDiagonally(kingMove,-debth,-debth,posY,posX,outputPossibleMoves);
            kingMove = true;
            SearchDiagonally(kingMove,debth,-debth,posY,posX,outputPossibleMoves);
            break;
        }
    }
    return outputPossibleMoves;
}

void ChessManager::GenerateStartPosition(int& colour) {
    std::vector<int> PawnLine(8,1);
    std::vector<int> EnemyPawnLine(8,-1);
    if(colour == -1){
        std::vector<int> TopLine{4,3,2,6,5,2,3,4};
        std::vector<int> EnemyTopLine{-4,-3,-2,-6,-5,-2,-3,-4};
        AllPositions[0] = EnemyTopLine;
        AllPositions[1] = EnemyPawnLine;
        AllPositions[6] = PawnLine;
        AllPositions[7] = TopLine;
    } else{
        std::vector<int> TopLine{4,3,2,5,6,2,3,4};
        std::vector<int> EnemyTopLine{-4,-3,-2,-5,-6,-2,-3,-4};
        AllPositions[0] = EnemyTopLine;
        AllPositions[1] = EnemyPawnLine;
        AllPositions[6] = PawnLine;
        AllPositions[7] = TopLine;
    }
}

void ChessManager::MakeMove(std::pair<int,int>& pos, std::pair<int,int>& target) {         //Сделать чтобы на сервер отправлялось
    //thisPlayerServer.SendDatagram(pos,target);
    CommitMove(pos,target);
}

void ChessManager::CommitMove(std::pair<int,int> pos, std::pair<int,int> target) {
    if(GetId(pos) > 0){
        EnPassant.clear();
    }
    if(GetId(pos) == -1 && target.first-pos.first > 1){
        EnPassant.emplace_back(pos.first+1,pos.second);
    }
    if(GetId(target) > 0){
        oneTeamPieces--;
    } else if(GetId(target) < 0){
        otherTeamPieces--;
    }
    if(GetId(pos) == 1 && (target.first == 0 || target.first == 7)){
        AllPositions[pos.first][pos.second] = 5;
    }
    if(GetId(pos) == -1 && (target.first == 0 || target.first == 7)){
        AllPositions[pos.first][pos.second] = -5;
    }
    bool EnPassantMove = false;
    if(GetId(target) == 0 && std::abs(GetId(pos)) == 1 && target.second != pos.second){
        EnPassantMove = true;
    }
    AllPositions[target.first][target.second] = AllPositions[pos.first][pos.second];
    AllPositions[pos.first][pos.second] = 0;
    if(EnPassantMove){
        AllPositions[target.first+GetId(target)][target.second] = 0;
        thisTimeManager->AllTimes[target.first+GetId(target)][target.second] = 0;
    }

    bool targetFound = false;
    for(int i = 0; i < thisTimeManager->PositionsTime.size(); i++){
        if(!targetFound && thisTimeManager->PositionsTime[i] == target){
            thisTimeManager->AllTimes[target.first][target.second] = 0;
            targetFound = true;
        }
        if(targetFound && i < thisTimeManager->PositionsTime.size()-1){
            thisTimeManager->PositionsTime[i] = thisTimeManager->PositionsTime[i+1];
        }
    }
    if(targetFound)
        thisTimeManager->PositionsTime.pop_back();

    thisTimeManager->AllTimes[target.first][target.second] = cdMove;
    thisTimeManager->PositionsTime.push_back(target);

    emit MoveCommited(pos, target, EnPassantMove);

    if(oneTeamPieces == 0) {
        emit GameEndedLose();
    }
    if(otherTeamPieces == 0) {
        emit GameEndedWin();
    }
}

bool ChessManager::pieceExists(std::pair<int,int>& pos) {
    if(AllPositions[pos.first][pos.second] > 0 && thisTimeManager->AllTimes[pos.first][pos.second] == 0){
        return true;
    }
    return false;
}

bool ChessManager::moveFair(int &posY, int &posX, std::vector<std::pair<int,int>>& possibleMoves) {
    for(auto& i: possibleMoves){
        if(posY == i.first && posX == i.second){
            return true;
        }
    }
    return false;
}
