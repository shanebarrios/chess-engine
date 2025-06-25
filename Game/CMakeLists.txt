find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_image CONFIG REQUIRED)

file(GLOB SRC_FILES
	"${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp")

add_executable(Game ${SRC_FILES})

add_custom_command(
	TARGET Game
	POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
	${CMAKE_CURRENT_SOURCE_DIR}/assets
	$<TARGET_FILE_DIR:Game>/assets
)

target_link_libraries(Game PRIVATE 
    ChessEngine 
    SDL2::SDL2 
    $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,SDL2_image::SDL2_image-static> 
    SDL2::SDL2main)
