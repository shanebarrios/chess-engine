cmake_minimum_required(VERSION 3.10)
project(ChessEngine)

file(GLOB SRC_FILES 
	"${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp")

add_library(ChessEngine ${SRC_FILES})

target_include_directories(ChessEngine 
	PUBLIC
		"${CMAKE_CURRENT_SOURCE_DIR}/src"
)