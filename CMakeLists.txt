cmake_minimum_required ( VERSION 3.17 )

# GENERAL SETUP
set ( CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMakeModules" )
set ( CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib" )
set ( CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib" )
set ( CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin" )
set ( CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DDEBUG" )

# PROJECTS
project ( game )
project ( engine )

# PACKAGES
find_package ( SDL2 REQUIRED )
find_package ( SDL2_ttf REQUIRED )

include_directories ( SDL2_ttf SDL2 PRIVATE "${CMAKE_SOURCE_DIR}/include" PRIVATE "${CMAKE_SOURCE_DIR}/src/engine" )

if (APPLE)
	include_directories ( "/opt/homebrew/include" )
	link_directories ( "/opt/homebrew/lib" "./lib/" )
endif()

# GAME
file(
	GLOB_RECURSE
	GAME_SRC
	"${CMAKE_SOURCE_DIR}/src/game/*.c"
	)

add_library ( game STATIC ${GAME_SRC} )

set_target_properties ( game PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib )

target_link_libraries ( game SDL2 ${SDL2_LIBRARIES} ecs )


# ENGINE
file(
	GLOB_RECURSE
	ENGINE_SRC
	"${CMAKE_SOURCE_DIR}/src/engine/*.c"
	)

add_executable ( engine ${ENGINE_SRC} )

target_link_libraries ( engine SDL2 ${SDL2_LIBRARIES} game ecs )

