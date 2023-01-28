cmake_minimum_required(VERSION 3.14)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (WIN32)
    project(RTChess LANGUAGES CXX)
elseif(UNIX)
    project(RTChess)
endif()

set(CMAKE_CONFIGURATION_TYPES "Release;RelWithDebInfo" CACHE STRING "" FORCE)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_PREFIX_PATH $ENV{QTDIR})

find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Network REQUIRED)

set(SOURCE_DIR "${CMAKE_CURRENT_BINARY_DIR}/Img"
               "${CMAKE_CURRENT_BINARY_DIR}/Visuals"
               "${CMAKE_CURRENT_BINARY_DIR}/Visuals/ServerLogic"
               "${CMAKE_CURRENT_BINARY_DIR}/Visuals/ServerLogic/CommonChessLogic"
               "${CMAKE_CURRENT_BINARY_DIR}/Visuals/ServerLogic/CommonChessLogic/TimeManager")

include_directories(${SOURCE_DIR})

file(GLOB_RECURSE SOURCES
        "${SOURCE_DIR}/*.ui"
        "${SOURCE_DIR}/*.qrc"
        "${SOURCE_DIR}/*.h"
        "${SOURCE_DIR}/*.cpp"
        )


set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS
        $<$<CONFIG:RELWITHDEBINFO>:QT_MESSAGELOGCONTEXT>
        )


set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_AUTOUIC_SEARCH_PATHS} ${FORMS_DIR})

if (WIN32)
    add_executable(RTChess WIN32 ${SOURCES})
elseif(UNIX)
    add_executable(RTChess ${SOURCES})
endif()

target_include_directories(RTChess PRIVATE ${SOURCE_DIR})

target_link_libraries(RTChess Qt5::Widgets)
target_link_libraries(RTChess Qt5::Core)
target_link_libraries(RTChess Qt5::Gui)
target_link_libraries(RTChess Qt5::Network)

install(TARGETS RTChess DESTINATION .)