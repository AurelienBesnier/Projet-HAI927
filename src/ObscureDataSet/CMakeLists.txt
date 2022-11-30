cmake_minimum_required (VERSION 3.12)
project (ObscureDataSet)

set(PROJECT_SOURCES
    obscuredataset.cpp
)

find_package( OpenCV REQUIRED )
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${PROJECT_SOURCES})
add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})
set_target_properties(${PROJECT_NAME}
    PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
)

target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS} Faceloc)
