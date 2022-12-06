cmake_minimum_required (VERSION 3.12)
project(Obscuration)

set(PROJECT_SOURCES
    obscuration.cpp
    obscuration.h
)
set(OTHER_SOURCES
    blackhead.cpp
    blur.cpp
    pixel.cpp
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
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})

add_executable(Blackhead blackhead.cpp)
add_executable(Blur blur.cpp)
add_executable(Pixel pixel.cpp)
target_link_libraries(Blackhead ${PROJECT_NAME} DisplaySystem)
target_link_libraries(Blur ${PROJECT_NAME} DisplaySystem)
target_link_libraries(Pixel ${PROJECT_NAME} DisplaySystem)
