cmake_minimum_required (VERSION 3.12)
project (GenPollingData)

set(PROJECT_SOURCES
    genpollingdata.cpp
)
set(PROJECT_HEADERS
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${PROJECT_SOURCES} ${PROJECT_HEADERS})
add_executable(${PROJECT_NAME} ${PROJECT_SOURCES} ${PROJECT_HEADERS})
set_target_properties(${PROJECT_NAME}
    PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
)
target_link_libraries(${PROJECT_NAME})
