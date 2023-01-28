#ifndef RTCHESS2_FRONTEND_H
#define RTCHESS2_FRONTEND_H

#include <vector>
#include <QMainWindow>
#include <QLabel>
#include <QLineEdit>
#include <QGridLayout>
#include <QPushButton>
#include <QScreen>
#include <QGuiApplication>
#include <QTextBrowser>
#include <QRegularExpression>
#include <QCheckBox>
#include <chrono>
#include <ctime>
#include <queue>
#include "ServerLogic/MyServer.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr) {}

    ~MainWindow() override;

    void RunGameMenu();
    void CreateTable();

    //тестирование
    //не имеют отношения к самой игре, просто существую для вызова хода в определенный момент времени
    bool ForTests = false;
    QWidget* testWidget = new QWidget;
private:
    class OneCellButton{
    public:
        class ChessButton: public QPushButton{
        public:
            ChessButton(int posY, int posX)
                :posY(posY), posX(posX) {
                cPos = posY + posX;
                if((cPos+1) % 2){
                    this->setStyleSheet("QPushButton { background-color: white; border: none; color: white }");
                } else{
                    this->setStyleSheet("QPushButton { background-color: green; border: none; color: green }");
                }
            }
        private:
            void enterEvent(QEvent *event) override{
                if((cPos+1) % 2){
                    this->setStyleSheet("QPushButton { background-color: white; border: 5px solid yellow; color: white }");
                } else{
                    this->setStyleSheet("QPushButton { background-color: green; border: 5px solid yellow; color: green }");
                }
            }
            void leaveEvent(QEvent *event) override{
                if((cPos+1) % 2){
                    this->setStyleSheet("QPushButton { background-color: white; border: none; color: white }");
                } else{
                    this->setStyleSheet("QPushButton { background-color: green; border: none; color: green }");
                }
            }
            int posY = 0;
            int posX = 0;
            int cPos = 0;
        };
        //наследник QPushButton, у которого появляется желтая рамка при наведении мыши

        OneCellButton(int posY, int posX);
        ~OneCellButton(){
            thisButton = nullptr;
            delete thisButton;
        }
        std::pair<int,int> GetPosition(){
            return {posY,posX};
        }
        ChessButton* thisButton;

    private:
        int posX = 0;
        int posY = 0;
    };
    //разделение шахматного поля на клетки
    //каждая клетка - кнопка фиксированного размера на определенной позиции

    bool isHost = false;
    //false - игрок не является хосьом, он не будет создавать сервер, отвечающий на запросы, каждый его ход будет сночала проверен хостом
    //true - игрок является хостом, на его стороне будет создан сервер, отвечающий на запросы, каждый его ход будет делаться мгновенно

    QFont txtFont;
    //стандартный шрифт приложения

    double height = QGuiApplication::screens()[0]->size().height();
    //высота экрана пользователя
    const QSize oneSquareSize = QSize(height/12,height/12);
    const QSize oneImageSize = QSize(height/15,height/15);
    const QSize oneSupportPartImageSize = QSize(height/24,height/24);
    const QSize widgetSize = QSize((height*2)/3,(height*5)/6);
    //подогнанные под экран пользователя размеры виджетов

    QWidget* centralWidget = new QWidget;

    QWidget* exitWidget;
    QLineEdit* passwordEnter;
    QPushButton* exitBtn = new QPushButton("X");

    QWidget* serverConnectWidget;
    void ShowConnectToServerMenu();
    //показывает меню подключения

    MyServer* thisServer;
    //создание элемента класса MyServer, в котором рассписана логика передачи данных между игроками

    QLabel* timerLabel;
    QLabel* timerLabel1;

    std::pair<int,int> GetThisButtonPosition(QString thisButtonName);
    std::vector<std::pair<int,int>> possibleMoves;
    //все возможные ходы, обновляется при выборе новой фигуры, или при отмене выбора
    bool pieceSelected = false;
    std::pair<int,int> selectedPiece;
    //posY, posX выбранной фигуры

    void DrawPossibleMoves();
    void ClearPossibleMoves();
    void DrawStartPosition();
    //отрисовывает стартовые фигры, создает пустые виджеты перезарядок фигур и вспомогательных элементов
    void UpdatePosition(std::pair<int,int>& pos, std::pair<int,int>& target, bool EnPassant);
    //обновляет картинки в необходимых местах

    QWidget* AllPiecesDrawn;
    QGridLayout* AllPiecesGridLayout;
    std::vector<std::vector<QLabel*>> allPictures = std::vector(8,std::vector<QLabel*>(8,0));

    int colour = 1;
    //1 - белые
    //2 - черные
    //по-умолчанию, 1 для хоста, 2 для неХоста

    QWidget* AllSupportPartsDrawn;
    QGridLayout* AllSupportPartsGridLayout;
    std::vector<std::vector<QLabel*>> allSupportParts = std::vector(8,std::vector<QLabel*>(8,0));

    QWidget* AllCooldownDrawn;
    QGridLayout* AllCooldownGridLayout;
    std::vector<std::vector<QLabel*>> allCooldowns = std::vector(8,std::vector<QLabel*>(8,0));
    void UpdateCooldowns();
    //обновление перезарядок

    void GoToMainMenu();
    void RestartAll();
    //перезагрузить доску, вернуться на стартовый экран

    QLabel* endgameMenu;
    QWidget* endWidget;
    QPushButton* restartBtn;
    //дизайн экрана окончания игры

    bool firstTime = true;
    bool isPaused = true;
    //true - игра остановлена, первые 5 секунд игроки ждут, потом isPaused = false

    //тестирование
    //не имеют отношения к самой игре, просто существую для вызова хода в определенный момент времени
    QTextBrowser* outputTest;
    QLabel* chosenFile;
    QPushButton* chooseFile;
    QCheckBox* changeSide;

    QString testIpText;

    QTcpServer* tcpSrv;
    QTcpSocket* tcpSck;
    quint16 sbSize1 = 0;
    quint16 sbSize2 = 0;

    qint64 startTime = 0;
    qint64 startTimeOther = 0;
    struct Command{
    public:
        Command(QString com){
            QStringList thisComList = com.split(" ");
            timeForMove = thisComList.takeFirst().toInt();
            QString posString = thisComList.takeFirst();
            QString targetString = thisComList.takeFirst();
            bool n = true;
            pos.first = 7 - (posString[1].unicode() - 49);
            pos.second = posString[0].unicode() - 65;
            if(pos.second > 7){
                pos.second = posString[0].unicode() - 97;
            }
            target.first = 7 - (targetString[1].unicode() - 49);
            target.second = targetString[0].unicode() - 65;
            if(target.second > 7){
                target.second = targetString[0].unicode() - 97;
            }
        }
        ~Command() = default;
        qint64 timeForMove = 0;
        std::pair<int,int> pos;
        std::pair<int,int> target;
    };

    void CommandCreation(QString com);
    std::vector<Command> allCommands;

    void SendToTestSocket(QString input);
    void SendToTestServer(qint64 time);

    void CreateCommandsBothSides();
    QString ExtractTextFromFile();
    QString allCommandsFromFile;
    bool sideChange = false;

    int curCommand = 0;
private slots:
    void SocketOnNewConnection();
    void OnNewConnection();
    //вызывается, когда к хосту подключается сокет
    void slotReadData();
    //вызывается, когда появилась новая информация для чтения

    void MainMenuButtonHandler();
    //логика кнопок стартового меню, и меню подключения
    void PlayButtonCellsHandler();
    //логика ходов на доске

    void UpdateAfterASecond();
    //вызывается каждую секунду
    //обновляет перезарядки и таймеры

    void DoAllTheJobAfterMove(std::pair<int,int>& pos, std::pair<int,int>& target, bool EnPassant);
    //делает все необходимое после хода с любой стороны

    void ShowWinMenu();
    void ShowLoseMenu();
    //экраны окончания игры

    //тестирование
    void SendButtonsHandler();
    void slotReadCommandsTestsSocket();
    void slotReadCommandsTestsServer();
    void OnNewConnectionTests();
    void SocketTestConnected();

    void TimeForCommandTest();
};


#endif //RTCHESS2_FRONTEND_H
