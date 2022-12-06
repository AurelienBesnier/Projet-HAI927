cmake_minimum_required (VERSION 3.12)
project(Tools)

set(PROJECT_SOURCES
    tools.h
    obscuredatasetfuncs.cpp
    annotatefuncs.cpp
    datasetfuncs.cpp
)
set(OTHER_SOURCES
    obscuredataset.cpp
    annotate.cpp
    gendataset.cpp
    genpollingdata.cpp
)

find_package( OpenCV REQUIRED )
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${PROJECT_SOURCES} ${OTHER_SOURCES})
add_library(${PROJECT_NAME} ${PROJECT_SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(${PROJECT_NAME}
    PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
)
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS} Faceloc Obscuration)

add_executable(ObscureDataSet obscuredataset.cpp)
target_link_libraries(ObscureDataSet ${PROJECT_NAME})
add_executable(Annotate annotate.cpp)
target_link_libraries(Annotate ${PROJECT_NAME})
add_executable(GenDataSet gendataset.cpp)
target_link_libraries(GenDataSet ${PROJECT_NAME})
add_executable(GenPollingData genpollingdata.cpp)
target_link_libraries(GenPollingData ${PROJECT_NAME})
