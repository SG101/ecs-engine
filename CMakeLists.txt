cmake_minimum_required ( VERSION 3.17 )

# GENERAL SETUP
set ( CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/lib" )
set ( CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/lib" )
set ( CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin" )

# PACKAGES
find_package ( SDL2 REQUIRED )

# GAME
project ( game )

file(
	GLOB_RECURSE
	GAME_SRC
	"${CMAKE_SOURCE_DIR}/src/game/*.c"
	)

add_library ( game STATIC ${GAME_SRC} )

set_target_properties ( game PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib )

target_link_libraries ( game ${SDL2_LIBRARIES} lib/libecs.a )
target_include_directories ( game PRIVATE ${SDL2_INCLUDE_DIRS} PRIVATE "${CMAKE_SOURCE_DIR}/include" PRIVATE "${CMAKE_SOURCE_DIR}/src/engine" )


# ENGINE
project ( engine )

file(
	GLOB_RECURSE
	ENGINE_SRC
	"${CMAKE_SOURCE_DIR}/src/engine/*.c"
	)

add_executable ( engine ${ENGINE_SRC} )

target_link_libraries ( engine ${SDL2_LIBRARIES} game lib/libecs.a )
target_include_directories ( engine PRIVATE ${SDL2_INCLUDE_DIRS} PRIVATE "${CMAKE_SOURCE_DIR}/include" )

