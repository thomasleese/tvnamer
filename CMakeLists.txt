cmake_minimum_required(VERSION 2.8.9)
project(tvnamer)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5Widgets REQUIRED)
find_package(Qt5Network REQUIRED)

qt5_wrap_ui(UI src/mainwindow.ui src/findshowdialogue.ui src/seasonwidget.ui)

add_executable(tvnamer src/main.cpp src/mainwindow.cpp src/findshowdialogue.cpp src/seasonwidget.cpp src/thetvdb.cpp ${UI})
qt5_use_modules(tvnamer Widgets Network)
