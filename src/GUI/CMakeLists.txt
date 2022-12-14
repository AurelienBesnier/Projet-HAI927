cmake_minimum_required (VERSION 3.12)
project(GUI)

set(PROJECT_SOURCES
    main.cpp
    MainWindow.cpp
    MainWindow.ui
    Commands.cpp
    FileSelector.cpp
    FileSelector.ui
    ToolWindow.cpp
    ToolWindow.ui
)

set(PROJECT_HEADERS
    MainWindow.h
    Commands.h
    FileSelector.h
    ToolWindow.h
)

find_package( OpenCV REQUIRED )
find_package( Qt5 REQUIRED COMPONENTS Widgets)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${PROJECT_SOURCES} ${PROJECT_HEADERS})
add_executable(${PROJECT_NAME} ${PROJECT_SOURCES} ${PROJECT_HEADERS})
set_target_properties(${PROJECT_NAME}
    PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
)

target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS} Qt5::Widgets fdeep Obscuration Tools)
