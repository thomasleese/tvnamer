set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5Network)
find_package(Qt5Widgets)

qt5_wrap_ui(UI_HEADERS mainwindow.ui findshowdialogue.ui seasonwidget.ui)

add_executable(tvnamer main.cpp mainwindow.cpp findshowdialogue.cpp seasonwidget.cpp thetvdb.cpp ${UI_HEADERS})
qt5_use_modules(tvnamer Network Widgets)
