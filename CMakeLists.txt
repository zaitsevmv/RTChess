cmake_minimum_required(VERSION 3.5)

project(RTChess VERSION 1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt5 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    Network
    REQUIRED)

find_package(Qt5 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    Network
    REQUIRED)

set(PROJECT_SOURCES
            main.cpp
            Visuals/Frontend.cpp
            Visuals/Frontend.h
            Visuals/Frontend.ui
            Visuals/ServerLogic/MyServer.cpp
            Visuals/ServerLogic/MyServer.h
            Visuals/ServerLogic/CommonChessLogic/ChessManager.h
            Visuals/ServerLogic/CommonChessLogic/ChessManager.cpp
            Visuals/ServerLogic/CommonChessLogic/TimeManager/timemanager.h
            Visuals/ServerLogic/CommonChessLogic/TimeManager/timemanager.cpp)

add_executable(RTChess
        Img.qrc
        ${PROJECT_SOURCES})

target_link_libraries(RTChess PRIVATE Qt5::Widgets
Qt5::Core
Qt5::Gui
Qt5::Network)