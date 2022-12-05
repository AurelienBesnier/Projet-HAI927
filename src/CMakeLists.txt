cmake_minimum_required (VERSION 3.12)
project (HAI927)

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

include(FetchContent)
FetchContent_Declare(
	FUNCTIONAL_PLUS
	GIT_REPOSITORY https://github.com/Dobiasd/FunctionalPlus.git
	GIT_TAG c64486270d0b6ed061d5322b826cc44a9f872f1d
)
FetchContent_Declare(
	FRUGALLY_DEEP
	GIT_REPOSITORY https://github.com/Dobiasd/frugally-deep.git
	GIT_TAG 1e22971834b86f19de3b706d54ccdd59138216e5
)
FetchContent_MakeAvailable(FUNCTIONAL_PLUS FRUGALLY_DEEP)

add_subdirectory(DisplaySystem)
add_subdirectory(Blur)
add_subdirectory(Pixel)
add_subdirectory(Blackhead)
add_subdirectory(Faceloc)
add_subdirectory(Annotate)
add_subdirectory(GenDataSet)
add_subdirectory(GenPollingData)
add_subdirectory(ObscureDataSet)
add_subdirectory(GUI)
