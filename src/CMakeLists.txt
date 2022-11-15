cmake_minimum_required (VERSION 3.12)
project (HAI927)

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

add_subdirectory(DisplaySystem)
add_subdirectory(Blur)
add_subdirectory(Pixel)
add_subdirectory(Blackhead)
add_subdirectory(Faceloc)
add_subdirectory(Annotate)
