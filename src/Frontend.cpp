#include <QApplication>
#include <QPushButton>
#include <QGridLayout>
#include <QLabel>
#include <QPainter>
#include <QDateTime>
#include <QThread>
#include <algorithm>
#include <QNetworkInterface>
#include <QFile>
#include <QFileDialog>

#include "Frontend.h"

MainWindow::~MainWindow() {

}

MainWindow::OneCellButton::OneCellButton(const int posY, const int posX)
        :posY(posY), posX(posX){
    QString thisBtnName = QString::number(posY) + QString::number(posX);
    thisButton = new ChessButton(posY, posX);
    thisButton->setMouseTracking(true);
    thisButton->setText(thisBtnName);
    thisButton->isFlat();
}

std::pair<int,int> MainWindow::GetThisButtonPosition(QString thisButtonName){
    int posY = thisButtonName.toInt() /10;
    int posX = thisButtonName.toInt() %10;
    return {posY,posX};
}

void MainWindow::CreateTable() {
    thisServer->thisChessManager->GenerateStartPosition(colour);
    connect(thisServer->thisChessManager->thisTimeManager->thisTimer, SIGNAL(timeout()),
            this, SLOT(UpdateAfterASecond()));

    connect(thisServer->thisChessManager, SIGNAL(MoveCommited(std::pair<int,int>&,std::pair<int,int>&,bool)),
            this, SLOT(DoAllTheJobAfterMove(std::pair<int,int>&,std::pair<int,int>&,bool)));
    connect(thisServer->thisChessManager, SIGNAL(GameEndedLose()), this, SLOT(ShowLoseMenu()));
    connect(thisServer->thisChessManager, SIGNAL(GameEndedWin()), this, SLOT(ShowWinMenu()));
    exitWidget->setVisible(false);

    centralWidget = nullptr;
    centralWidget = new QWidget;
    this->setCentralWidget(centralWidget);
    QGridLayout* chessBoard = new QGridLayout;
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            OneCellButton btn = OneCellButton(i,j);
            btn.thisButton->setFixedSize(oneSquareSize);
            connect(btn.thisButton, SIGNAL(clicked()), this,
                    SLOT(PlayButtonCellsHandler()));
            btn.thisButton->setMouseTracking(true);
            chessBoard->addWidget(btn.thisButton, btn.GetPosition().first+1,btn.GetPosition().second);
        }
    }
    timerLabel = new QLabel;
    timerLabel1 = new QLabel;
    timerLabel->setAlignment(Qt::Alignment(Qt::AlignCenter));
    timerLabel1->setAlignment(Qt::Alignment(Qt::AlignCenter));

    chessBoard->addWidget(timerLabel1,0,7);
    chessBoard->addWidget(timerLabel,9,7);
    chessBoard->setSpacing(0);
    centralWidget->setLayout(chessBoard);
    QPoint mid = QPoint(0,0);
    centralWidget->mapToGlobal(mid);
    centralWidget->setFixedSize(widgetSize);
    DrawStartPosition();
    UpdateAfterASecond();
    startTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
}

void MainWindow::RunGameMenu() {
    txtFont.setPixelSize(height/40);
    QGridLayout* mainMenuLayout = new QGridLayout;
    QPushButton* mainBtn1 = new QPushButton("Начать игру");
    QPushButton* mainBtn2 = new QPushButton("Присоединиться");
    int k = 0;
    for(auto& i: {mainBtn1, mainBtn2}){
        i->setFont(txtFont);
        i->setMinimumSize(height/6,height/12);
        connect(i, SIGNAL(clicked()),this, SLOT(MainMenuButtonHandler()));
        mainMenuLayout->addWidget(i,1,k,Qt::AlignCenter);
        k++;
    }
    if(firstTime){
        exitBtn->setFont(txtFont);
        exitBtn->setFixedSize(height/18,height/18);
        connect(exitBtn, SIGNAL(clicked()), this, SLOT(MainMenuButtonHandler()));
        QGridLayout* exitLayout = new QGridLayout;
        exitLayout->addWidget(exitBtn);
        exitWidget = new QWidget;
        exitWidget->setFixedSize(height/15,height/15);
        exitWidget->setLayout(exitLayout);
    }
    centralWidget->setLayout(mainMenuLayout);
    this->setCentralWidget(centralWidget);
    centralWidget->setFixedSize(widgetSize);
    this->layout()->addWidget(exitWidget);

    //UI тестовое меню
    if(ForTests){
        QHBoxLayout* testHBoxLayout = new QHBoxLayout;
        QHBoxLayout* testHBoxLayout1 = new QHBoxLayout;
        QVBoxLayout* testVBoxLayout = new QVBoxLayout;
        chosenFile = new QLabel;
        chosenFile->setText("Файл не выбран");
        outputTest = new QTextBrowser;
        chooseFile = new QPushButton("Выбрать файл");
        connect(chooseFile, SIGNAL(clicked()), this, SLOT(SendButtonsHandler()));
        changeSide = new QCheckBox("Поменять стороны");
        testHBoxLayout1->addWidget(chooseFile);
        testHBoxLayout1->addWidget(changeSide);
        testVBoxLayout->addWidget(chosenFile);
        testVBoxLayout->addLayout(testHBoxLayout1);
        testHBoxLayout->addLayout(testVBoxLayout);
        testHBoxLayout->addWidget(outputTest);
        testWidget->setLayout(testHBoxLayout);

        tcpSrv = new QTcpServer(this);
        tcpSrv->listen(QHostAddress::Any,50000);
        connect(tcpSrv, SIGNAL(newConnection()), this, SLOT(OnNewConnectionTests()));
    } else{
        tcpSck = new QTcpSocket(this);
    }
}

void MainWindow::MainMenuButtonHandler() {
    QString btnText = ((QPushButton*)sender())->text();
    if(btnText == "Начать игру"){
        isHost = true;
        colour = 1;
        ShowConnectToServerMenu();
    } else if(btnText == "Присоединиться") {
        isHost = false;
        colour = -1;
        ShowConnectToServerMenu();
    } else if(btnText == "Начать"){
        thisServer = new MyServer(isHost,1);
        QPushButton* thisBtn = (QPushButton*)sender();
        thisBtn->setFlat(true);
        thisBtn->setText("Ожидание подключения...");
        QString password;
        for(int i = 0; i < 8; i+=2){
            password += QString::number(passwordEnter->text()[i].unicode()-65);
            password += passwordEnter->text()[i+1];
            if(i != 6){
                password += ".";
            }
        }
        testIpText = password;
        QHostAddress targetAddress = QHostAddress(password);

        thisServer->thisSocket->connectToHost(targetAddress, 33333);

        connect(thisServer->thisSocket, SIGNAL(readyRead()), this, SLOT(slotReadData()));
        connect(thisServer->thisSocket, SIGNAL(disconnected()), thisServer->thisSocket, SLOT(deleteLater()));
        connect(thisServer->thisSocket, SIGNAL(connected()), this, SLOT(SocketOnNewConnection()));

        passwordEnter->setVisible(false);
        passwordEnter = nullptr;
    } else if(btnText == "Начать заново"){
        RestartAll();
    } else if(btnText == "X"){
        GoToMainMenu();
    }
}

void MainWindow::ShowConnectToServerMenu() {
    QGridLayout* connectMenuLayout = new QGridLayout;
    serverConnectWidget = new QWidget;
    exitWidget = nullptr;
    QGridLayout* exitLayout = new QGridLayout;
    exitLayout->addWidget(exitBtn);
    exitWidget = new QWidget;
    exitWidget->setFixedSize(height/15,height/15);
    exitWidget->setLayout(exitLayout);
    if(isHost){
        thisServer = new MyServer(isHost,0);
        QLabel* waitForConnection = new QLabel;
        waitForConnection->setTextInteractionFlags(Qt::TextSelectableByMouse);
        QTcpSocket localServerIp;

        localServerIp.connectToHost("8.8.8.8", 53);
        localServerIp.waitForConnected();

        waitForConnection->setFont(txtFont);
        QStringList thisIpStringList = localServerIp.localAddress().toString().split(".");
        testIpText = localServerIp.localAddress().toString();
        QString password;
        for(auto& num: thisIpStringList){
            int n = num.toInt();
            password += QString(n / 10 + 65);
            password += QString::number(n % 10);
        }
        waitForConnection->setText("Пароль: " + password);
        connect(thisServer->ServerHost, SIGNAL(newConnection()), this, SLOT(OnNewConnection()));

        waitForConnection->setMinimumSize(height/4,height/10);
        connectMenuLayout->addWidget(waitForConnection,0,0,Qt::AlignCenter);
    } else{
        thisServer = nullptr;
        passwordEnter = new QLineEdit;
        passwordEnter->setFont(txtFont);
        passwordEnter->setFixedSize(height/3,height/12);
        QPushButton* enterBtn = new QPushButton;
        enterBtn->setFont(txtFont);
        enterBtn->setText("Начать");
        connect(enterBtn, SIGNAL(clicked()),this, SLOT(MainMenuButtonHandler()));

        enterBtn->setMinimumSize(height/4,height/10);
        connectMenuLayout->addWidget(passwordEnter,0,0,Qt::AlignCenter);
        connectMenuLayout->addWidget(enterBtn,1,0,Qt::AlignCenter);
    }
    serverConnectWidget->setLayout(connectMenuLayout);
    serverConnectWidget->setFixedSize(widgetSize);
    this->setCentralWidget(serverConnectWidget);
    this->layout()->addWidget(exitWidget);
}

void MainWindow::PlayButtonCellsHandler() {
    if(!isPaused){
        QString positionString = ((QPushButton*)sender())->text();
        std::pair<int,int> positionPair = GetThisButtonPosition(positionString);;
        if(!pieceSelected){
            if(thisServer->thisChessManager->pieceExists(positionPair)){
                selectedPiece = positionPair;
                possibleMoves = thisServer->thisChessManager->FindPossibleMoves(positionPair.first,positionPair.second);
                DrawPossibleMoves();
                pieceSelected = true;
            } else{
                pieceSelected = false;
            }
        } else{
            if(thisServer->thisChessManager->moveFair(positionPair.first,positionPair.second,possibleMoves)){ //ход
                thisServer->MakeMove(selectedPiece,positionPair); //сначала на сервер, а потом комит
                ClearPossibleMoves();
                pieceSelected = false;
            } else if(thisServer->thisChessManager->AllPositions[positionPair.first][positionPair.second] > 0
                      && thisServer->thisChessManager->pieceExists(positionPair)){
                ClearPossibleMoves();
                selectedPiece = positionPair;
                possibleMoves = thisServer->thisChessManager->FindPossibleMoves(positionPair.first,positionPair.second);
                DrawPossibleMoves();
                pieceSelected = true;
            }
            else{
                ClearPossibleMoves();
                possibleMoves.clear();
                pieceSelected = false;
            }
        }
    }
}

void MainWindow::DrawStartPosition() {
    AllCooldownDrawn = new QWidget;
    AllPiecesDrawn = new QWidget;
    AllSupportPartsDrawn = new QWidget;
    AllPiecesGridLayout = new QGridLayout;
    AllSupportPartsGridLayout = new QGridLayout;
    AllCooldownGridLayout = new QGridLayout;
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            allPictures[i][j] = new QLabel;
            allSupportParts[i][j] = new QLabel;
            allCooldowns[i][j] = new QLabel;

            QString path = ":/Img/" + QString::number(colour*thisServer->thisChessManager->AllPositions[i][j]) + ".png";
            QImage img(path);
            QImage img1(":/Img/0.png");
            QImage img2(":/Img/blue.png");

            allSupportParts[i][j]->setFixedSize(oneSquareSize);
            allSupportParts[i][j]->setPixmap(QPixmap::fromImage(img1.scaled(0,0,
                                                                       Qt::KeepAspectRatio,Qt::FastTransformation)));

            allCooldowns[i][j]->setFixedSize(oneSquareSize);
            allCooldowns[i][j]->setPixmap(QPixmap::fromImage(img2.scaled(0,0,
                                                                       Qt::KeepAspectRatio,Qt::FastTransformation)));

            allPictures[i][j]->setFixedSize(oneSquareSize);
            if(thisServer->thisChessManager->AllPositions[i][j] == 0){
                allPictures[i][j]->setPixmap(QPixmap::fromImage(img.scaled(0,0,
                                                                  Qt::KeepAspectRatio,Qt::FastTransformation)));
            } else{
                allPictures[i][j]->setPixmap(QPixmap::fromImage(img.scaled(oneImageSize,
                                                                 Qt::KeepAspectRatio,Qt::FastTransformation)));
            }
            allPictures[i][j]->setAlignment(Qt::AlignCenter);
            allPictures[i][j]->setAttribute(Qt::WA_TransparentForMouseEvents);
            AllPiecesGridLayout->addWidget(allPictures[i][j], i+1,j);

            allCooldowns[i][j]->setAlignment(Qt::AlignCenter);
            allCooldowns[i][j]->setAttribute(Qt::WA_TransparentForMouseEvents);
            AllCooldownGridLayout->addWidget(allCooldowns[i][j], i+1,j);

            allSupportParts[i][j]->setAlignment(Qt::AlignCenter);
            allSupportParts[i][j]->setAttribute(Qt::WA_TransparentForMouseEvents);
            AllSupportPartsGridLayout->addWidget(allSupportParts[i][j], i+1,j);
        }
    }

    QVector<QLabel*> blankLabels = QVector<QLabel*>{new QLabel,new QLabel, new QLabel, new QLabel, new QLabel, new QLabel};
    AllPiecesGridLayout->addWidget(blankLabels[0],0,0);
    AllPiecesGridLayout->addWidget(blankLabels[1],9,0);
    AllSupportPartsGridLayout->addWidget(blankLabels[2],0,0);
    AllSupportPartsGridLayout->addWidget(blankLabels[3],9,0);
    AllCooldownGridLayout->addWidget(blankLabels[4],0,0);
    AllCooldownGridLayout->addWidget(blankLabels[4],9,0);

    AllPiecesGridLayout->setSpacing(0);
    AllPiecesDrawn->setLayout(AllPiecesGridLayout);
    QPoint mid = QPoint(0,0);
    AllPiecesDrawn->mapToGlobal(mid);
    AllPiecesDrawn->setFixedSize(widgetSize);
    AllPiecesDrawn->setAttribute(Qt::WA_TransparentForMouseEvents);
    this->layout()->addWidget(AllPiecesDrawn);

    AllSupportPartsGridLayout->setSpacing(0);
    AllSupportPartsDrawn->setLayout(AllSupportPartsGridLayout);
    AllSupportPartsDrawn->mapToGlobal(mid);
    AllSupportPartsDrawn->setFixedSize(widgetSize);
    AllSupportPartsDrawn->setAttribute(Qt::WA_TransparentForMouseEvents);
    this->layout()->addWidget(AllSupportPartsDrawn);

    AllCooldownGridLayout->setSpacing(0);
    AllCooldownDrawn->setLayout(AllCooldownGridLayout);
    AllCooldownDrawn->mapToGlobal(mid);
    AllCooldownDrawn->setFixedSize(widgetSize);
    AllCooldownDrawn->setAttribute(Qt::WA_TransparentForMouseEvents);
    this->layout()->addWidget(AllCooldownDrawn);
}

void MainWindow::UpdatePosition(std::pair<int,int>& pos, std::pair<int,int>& target, bool EnPassant) {
    for(auto& i: {pos,target}){
        QString path = ":/Img/" + QString::number(colour*thisServer->thisChessManager->AllPositions[i.first][i.second]) + ".png";
        QImage img(path);
        if(thisServer->thisChessManager->AllPositions[i.first][i.second] == 0){
            allPictures[i.first][i.second]->setPixmap(QPixmap::fromImage(img.scaled(0,0,
                                                                       Qt::KeepAspectRatio,Qt::FastTransformation)));
        } else{
            allPictures[i.first][i.second]->setPixmap(QPixmap::fromImage(img.scaled(oneImageSize,
                                                                       Qt::KeepAspectRatio,Qt::FastTransformation)));
        }
    }
    if(EnPassant){
        QImage img(":/Img/0.png");
        allPictures[target.first+colour][target.second]->setPixmap(QPixmap::fromImage(img.scaled(0,0,
                                                                   Qt::KeepAspectRatio,Qt::FastTransformation)));
    }
}

void MainWindow::DrawPossibleMoves() {
    QImage img(":/Img/a.png");
    for(auto &i: possibleMoves){
        if(thisServer->thisChessManager->AllPositions[i.first][i.second] == 0){
            allSupportParts[i.first][i.second]->setPixmap(QPixmap::fromImage(img.scaled(oneSupportPartImageSize,
                                                                       Qt::KeepAspectRatio,Qt::FastTransformation)));
        } else{
            allSupportParts[i.first][i.second]->setPixmap(QPixmap::fromImage(img.scaled(oneSquareSize,
                                                                       Qt::KeepAspectRatio,Qt::FastTransformation)));
        }
    }
}

void MainWindow::ClearPossibleMoves() {
    QImage img(":/Img/0.png");
    for(auto& i: possibleMoves){
        allSupportParts[i.first][i.second]->setPixmap(QPixmap::fromImage(img.scaled(0,0)));
    }
}

QString CalculateTime(int timeSecInt){
    QString time;
    if(timeSecInt/60 < 10){
        time += "0";
    }
    time += QString::number(timeSecInt/60) + ":";
    if(timeSecInt%60 < 10){
        time += "0";
    }
    time += QString::number(timeSecInt%60);
    return time;
}

void MainWindow::UpdateAfterASecond() {
    int timeSecInt = thisServer->thisChessManager->thisTimeManager->timer;
    QString time;
    if(timeSecInt < 0){
        time += "-";
        time += CalculateTime(-timeSecInt);
    } else {
        time = CalculateTime(timeSecInt);
    }
    timerLabel->setText(time);
    timerLabel1->setText(time);
    timerLabel->setFont(txtFont);
    timerLabel1->setFont(txtFont);
    thisServer->thisChessManager->thisTimeManager->FixedUpdateCoolDown();
    if(timeSecInt >= 0){
        UpdateCooldowns();
        if(isPaused){
            isPaused = false;
        }
    }
}

void MainWindow::UpdateCooldowns() {
    QImage img2(":/Img/blue.png");
    for(auto& i: thisServer->thisChessManager->thisTimeManager->PositionsTime){
        allCooldowns[i.first][i.second]->setPixmap(QPixmap::fromImage(
                img2.scaled(height/12,(height*thisServer->thisChessManager->thisTimeManager->AllTimes[i.first][i.second])/(60),
                Qt::IgnoreAspectRatio,Qt::FastTransformation)));
    }
}

void MainWindow::GoToMainMenu()
{
    serverConnectWidget = nullptr;
    firstTime = false;
    if(thisServer != nullptr){
        if(isHost)
            thisServer->ServerHost->deleteLater();
        else{
            thisServer->thisSocket->deleteLater();
        }
    }
    thisServer = nullptr;
    centralWidget = new QWidget;
    RunGameMenu();
    isHost = false;
}

void MainWindow::RestartAll()
{
    firstTime = true;
    isPaused = true;
    if(endWidget != nullptr){
        endgameMenu->setVisible(false);
        restartBtn->setVisible(false);
    }
    endgameMenu = nullptr;
    endWidget = nullptr;
    exitWidget = nullptr;

    restartBtn = nullptr;
    AllCooldownDrawn = nullptr;
    AllPiecesDrawn = nullptr;
    AllSupportPartsDrawn = nullptr;
    serverConnectWidget = nullptr;
    if(isHost)
        thisServer->ServerHost->deleteLater();

    thisServer = nullptr;
    timerLabel = nullptr;
    timerLabel1 = nullptr;
    AllPiecesGridLayout = nullptr;
    AllSupportPartsGridLayout = nullptr;
    AllCooldownGridLayout = nullptr;
    if(allCooldowns[0][0] != nullptr){
        for(int i = 0; i < 8; i++){
            for(int j = 0; j < 8; j++){
                allCooldowns[i][j]->setVisible(false);
                allCooldowns[i][j] = new QLabel;
                allPictures[i][j]->setVisible(false);
                allPictures[i][j] = new QLabel;
                allSupportParts[i][j]->setVisible(false);
                allSupportParts[i][j] = new QLabel;
            }
        }
    }

    centralWidget = new QWidget;
    RunGameMenu();
}

void MainWindow::CommandCreation(QString com)
{
    Command newCommand = Command(com);
    qint64 timeNow = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
    allCommands.push_back(newCommand);
    QTimer::singleShot(startTime - timeNow + newCommand.timeForMove, this, SLOT(TimeForCommandTest()));
}

void MainWindow::SendToTestSocket(QString input)
{
    QByteArray arrBlock;
    arrBlock.clear();
    QDataStream out(&arrBlock,QIODevice::ReadWrite);
    out.setVersion(QDataStream::Qt_5_0);

    out << qint16(0) << input;
    out.device()->seek(0);
    out << qint16(arrBlock.size() - sizeof(qint16));

    tcpSck->write(arrBlock);
}

void MainWindow::SendToTestServer(qint64 time)
{
    QByteArray arrBlock;
    arrBlock.clear();
    QDataStream out(&arrBlock,QIODevice::ReadWrite);
    out.setVersion(QDataStream::Qt_5_0);

    out << qint16(0) << time;
    out.device()->seek(0);
    out << qint16(arrBlock.size() - sizeof(qint16));

    tcpSck->write(arrBlock);
}

void MainWindow::SocketOnNewConnection()
{
    connect(thisServer->thisSocket, SIGNAL(disconnected()), this, SLOT(ShowWinMenu()));
    thisServer->StartGame();
    CreateTable();
    if(!ForTests){
        tcpSck->connectToHost(QHostAddress(testIpText), 50000);
        connect(tcpSck, SIGNAL(connected()), this, SLOT(SocketTestConnected()));
        connect(tcpSck, SIGNAL(readyRead()), this, SLOT(slotReadCommandsTestsSocket()));
        connect(tcpSck, SIGNAL(disconnected()), tcpSck, SLOT(deleteLater()));
    }
}

void MainWindow::OnNewConnection() {
    thisServer->thisSocket = thisServer->ServerHost->nextPendingConnection();
    connect(thisServer->thisSocket, SIGNAL(disconnected()), this, SLOT(ShowWinMenu()));
    connect(thisServer->thisSocket, SIGNAL(readyRead()), this, SLOT(slotReadData()));
    connect(thisServer->thisSocket, SIGNAL(disconnected()), thisServer->thisSocket, SLOT(deleteLater()));
    thisServer->StartGame();
    CreateTable();
    if(!ForTests){
        tcpSck->connectToHost(QHostAddress(testIpText), 50000);
        connect(tcpSck, SIGNAL(connected()), this, SLOT(SocketTestConnected()));
        connect(tcpSck, SIGNAL(readyRead()), this, SLOT(slotReadCommandsTestsSocket()));
        connect(tcpSck, SIGNAL(disconnected()), tcpSck, SLOT(deleteLater()));
    }
    thisServer->ServerHost->close();
}

void MainWindow::slotReadData() {
    thisServer->ReadData();
}

void MainWindow::DoAllTheJobAfterMove(std::pair<int,int>& pos, std::pair<int,int>& target, bool EnPassant){
    UpdatePosition(pos,target,EnPassant);
    UpdateCooldowns();

    if(std::find(possibleMoves.begin(), possibleMoves.end(), target) != possibleMoves.end()
            || std::find(possibleMoves.begin(), possibleMoves.end(), pos) != possibleMoves.end()){
        ClearPossibleMoves();
        possibleMoves.clear();
    }
}

void MainWindow::ShowWinMenu()
{
    disconnect(thisServer->thisSocket, SIGNAL(disconnected()), this, SLOT(ShowWinMenu()));
    endWidget = new QWidget;
    disconnect(thisServer->thisChessManager->thisTimeManager->thisTimer, SIGNAL(timeout()),
            this, SLOT(UpdateAfterASecond()));

    endgameMenu = new QLabel;
    endgameMenu->setFixedSize(widgetSize);
    QImage memeHaha(":/Img/win.png");
    endgameMenu->setPixmap(QPixmap::fromImage(memeHaha.scaled(widgetSize,
                        Qt::KeepAspectRatio,Qt::FastTransformation)));
    this->layout()->addWidget(endgameMenu);
    QGridLayout* endLayout = new QGridLayout;
    restartBtn = new QPushButton("Начать заново");
    connect(restartBtn, SIGNAL(pressed()), this, SLOT(MainMenuButtonHandler()));
    restartBtn->setFont(txtFont);
    QPoint mid = QPoint(0,0);
    restartBtn->mapToGlobal(mid);
    restartBtn->setFixedSize(height/4,height/10);
    QLabel* blank = new QLabel;
    blank->setFixedSize(height/4,height/10);
    endLayout->addWidget(blank,0,0);
    endLayout->addWidget(restartBtn,1,0);
    endWidget->setFixedSize(widgetSize);
    endWidget->setLayout(endLayout);
    this->layout()->addWidget(endWidget);
}

void MainWindow::ShowLoseMenu()
{
    disconnect(thisServer->thisSocket, SIGNAL(disconnected()), this, SLOT(ShowWinMenu()));
    endWidget = new QWidget;
    disconnect(thisServer->thisChessManager->thisTimeManager->thisTimer, SIGNAL(timeout()),
            this, SLOT(UpdateAfterASecond()));

    endgameMenu = new QLabel;
    endgameMenu->setFixedSize(widgetSize);
    QImage memeHaha(":/Img/lose.png");
    endgameMenu->setPixmap(QPixmap::fromImage(memeHaha.scaled(widgetSize,
                        Qt::KeepAspectRatio,Qt::FastTransformation)));
    this->layout()->addWidget(endgameMenu);
    QGridLayout* endLayout = new QGridLayout;
    restartBtn = new QPushButton("Начать заново");
    connect(restartBtn, SIGNAL(pressed()), this, SLOT(MainMenuButtonHandler()));
    restartBtn->setFont(txtFont);
    QPoint mid = QPoint(0,0);
    restartBtn->mapToGlobal(mid);
    restartBtn->setFixedSize(height/4,height/10);
    QLabel* blank = new QLabel;
    blank->setFixedSize(height/4,height/10);
    endLayout->addWidget(blank,0,0);
    endLayout->addWidget(restartBtn,1,0);
    endWidget->setFixedSize(widgetSize);
    endWidget->setLayout(endLayout);
    this->layout()->addWidget(endWidget);
}

void MainWindow::SendButtonsHandler()
{
    QString btnText = ((QPushButton*)sender())->text();
    if(btnText == "Выбрать файл"){
        allCommandsFromFile = ExtractTextFromFile();
    } if(btnText == "Поменять стороны"){
        if(sideChange){
            sideChange = false;
        } else{
            sideChange = true;
        }
    }
}

void MainWindow::CreateCommandsBothSides(){
    QStringList commandListTwoParts = allCommandsFromFile.split("\n==\n");
    QString allCommandsOne, allCommandsTwo;
    if(!changeSide->isChecked()){
        allCommandsOne = commandListTwoParts[0];
        allCommandsTwo = commandListTwoParts[1];
    } else{
        allCommandsOne = commandListTwoParts[1];
        allCommandsTwo = commandListTwoParts[0];
    }

    QStringList thisComList = allCommandsOne.split("\n");
    while(!thisComList.empty()){
        QString singleCommand = thisComList.takeFirst();
        CommandCreation(singleCommand);
    }

    SendToTestSocket(allCommandsTwo);
    outputTest->setText(outputTest->toPlainText() + allCommandsOne);
    outputTest->setText(outputTest->toPlainText() + "\nКоманды игроку 2:\n" + allCommandsTwo);
}

QString MainWindow::ExtractTextFromFile() {
    QString filePath;
    filePath = QFileDialog::getOpenFileName(testWidget, "Выбрать файл для тестирования","","All Files (*.txt)");
    QFile comFile(filePath);
    if (!comFile.open(QIODevice::ReadOnly | QIODevice::Text))
        return "";
    chosenFile->setText("Выбран файл: " + filePath);
    QTextStream in(&comFile);
    QString fileData;
    while (!in.atEnd()) {
        fileData = in.readAll();
    }
    return fileData;
}

void MainWindow::slotReadCommandsTestsSocket()
{
    QString commandLine = 0;
    QDataStream in(tcpSck);
    in.setVersion(QDataStream::Qt_5_0);

    for(;;){
        if(sbSize2 == 0){
            if(tcpSck->bytesAvailable() < sizeof(quint16)){
                break;
            }
            in >> sbSize2;
        }
        if(tcpSck->bytesAvailable() < sbSize2){
            break;
        }

        in >> commandLine;
        sbSize1 = 0;

        QStringList thisComList = commandLine.split("\n");
        while(!thisComList.empty()){
            QString singleCommand = thisComList.takeFirst();
            CommandCreation(singleCommand);
        }
    }
}

void MainWindow::slotReadCommandsTestsServer()
{
    qint64 curTimeGuest = 0;
    QDataStream in(tcpSck);
    in.setVersion(QDataStream::Qt_5_0);

    for(;;){
        if(sbSize1 == 0){
            if(tcpSck->bytesAvailable() < sizeof(quint16)){
                break;
            }
            in >> sbSize1;
        }
        if(tcpSck->bytesAvailable() < sbSize1){
            break;
        }

        in >> curTimeGuest;
        sbSize1 = 0;
    }
}

void MainWindow::OnNewConnectionTests()
{
    tcpSck = tcpSrv->nextPendingConnection();
    connect(tcpSck, SIGNAL(readyRead()), this, SLOT(slotReadCommandsTestsServer()));
    connect(tcpSck, SIGNAL(disconnected()), tcpSck, SLOT(deleteLater()));
    CreateCommandsBothSides();
}

void MainWindow::SocketTestConnected()
{
    SendToTestServer(startTime);
}

void MainWindow::TimeForCommandTest()
{
    Command thisCom = allCommands[curCommand];
    if(thisServer->thisChessManager->pieceExists(thisCom.pos)){
        possibleMoves = thisServer->thisChessManager->FindPossibleMoves(thisCom.pos.first,thisCom.pos.second);
        if(std::find(possibleMoves.begin(), possibleMoves.end(), thisCom.target) != possibleMoves.end()){
            thisServer->MakeMove(thisCom.pos,thisCom.target);
            possibleMoves.clear();
            curCommand++;
        }
    }
}
