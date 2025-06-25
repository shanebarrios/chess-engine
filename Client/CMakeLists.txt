find_package(CURL CONFIG QUIET)

if(NOT CURL_FOUND)
    find_package(CURL REQUIRED)
endif()

find_package(nlohmann_json CONFIG REQUIRED)

file(GLOB SRC_FILES
	"${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp")

add_executable(Client ${SRC_FILES})

target_link_libraries(Client PRIVATE ChessEngine CURL::libcurl nlohmann_json::nlohmann_json)
