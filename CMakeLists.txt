cmake_minimum_required(VERSION 3.14)

if (WIN32)
project(RTChess LANGUAGES CXX)
elseif(UNIX)
project(RTChess)
endif()

set(CMAKE_CONFIGURATION_TYPES "Release;RelWithDebInfo" CACHE STRING "" FORCE)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

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

set(IMG_DIR ${CMAKE_SOURCE_DIR}/images)
set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)

include_directories(${INCLUDE_DIR})
include_directories(${IMG_DIR})
include_directories(${SOURCE_DIR})

file(GLOB_RECURSE SOURCES
        ${IMG_DIR}/*.qrc
        ${INCLUDE_DIR}/*.h
        ${SOURCE_DIR}/*.cpp
)

set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS
$<$<CONFIG:RELWITHDEBINFO>:QT_MESSAGELOGCONTEXT>
)

if (WIN32)
add_executable(RTChess WIN32 ${SOURCES})
elseif(UNIX)
add_executable(RTChess ${SOURCES})
endif()

target_include_directories(RTChess PRIVATE ${INCLUDE_DIR})
target_include_directories(RTChess PRIVATE ${SOURCE_DIR})
target_include_directories(RTChess PRIVATE ${IMG_DIR})

target_link_libraries(RTChess Qt5::Widgets)
target_link_libraries(RTChess Qt5::Core)
target_link_libraries(RTChess Qt5::Gui)
target_link_libraries(RTChess Qt5::Network)

install(TARGETS RTChess DESTINATION .)