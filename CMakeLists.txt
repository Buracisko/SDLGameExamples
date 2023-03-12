cmake_minimum_required(VERSION 3.13)

project("SDL Game Example" VERSION 1.0 LANGUAGES CXX)

# Sources
set(CORE_SOURCES
	src/engine.h
	src/engine.cpp
)

source_group("Headers" FILES
	src/engine.h
)

# Set a default build type if none was specified
# Use -DCMAKE_BUILD_TYPE=XXX to se build type
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
	message("Setting build type to 'Debug' as none was specified.")
	set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
	# Set the possible values of build type for cmake-gui
	set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release")
endif()

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

if (MSVC)
	add_executable(Example1 WIN32 ${CORE_SOURCES} "src/example1/example1.cpp")
	add_executable(Example2 WIN32 ${CORE_SOURCES} "src/example2/example2.cpp")
	target_include_directories(Example1 PRIVATE "src")
	target_include_directories(Example2 PRIVATE "src")
	# warning level 3
	# TODO: Set it for all targets
	target_compile_options(Example1 PRIVATE /W3)

	list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/sdl2)

	# Find SDL2 and its libraries
	find_package(SDL2 REQUIRED)
	find_package(SDL2_image REQUIRED)
	find_package(SDL2_mixer REQUIRED)
	find_package(SDL2_ttf REQUIRED)

	target_link_libraries(Example1 PRIVATE SDL2::Core SDL2::Main SDL2::Image SDL2::Mixer SDL2::TTF)
	target_link_libraries(Example2 PRIVATE SDL2::Core SDL2::Main SDL2::Image SDL2::Mixer SDL2::TTF)
else()
	# TODO: LINUX
	# SDL2 libraries
	add_subdirectory(libraries/SDL2)
	add_subdirectory(libraries/SDL_mixer)
	add_subdirectory(libraries/SDL_ttf)
	# Add alias if its still missing
	add_library(SDL2::ttf ALIAS SDL2_ttf)

	# SDL_image Linux hotfix building
	if (NOT MSVC)
	add_compile_options(-fPIC)
	endif()
	add_subdirectory(libraries/SDL_image)
	add_executable(${PROJECT_NAME} ${SOURCES})

	# lots of warnings and all warnings as errors
	target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic -Werror -Wconversion -Wshadow -Wno-unused-parameter)

	target_link_libraries(${PROJECT_NAME} PRIVATE
		SDL2::SDL2
		SDL2::SDL2main
		SDL2::image
		SDL2::ttf
		SDL2::mixer
	)
endif()

if(MSVC)
	add_custom_command(
		TARGET Example1 POST_BUILD 
			COMMAND ${CMAKE_COMMAND} -E copy_directory
			"${CMAKE_CURRENT_SOURCE_DIR}/${SDL2_PATH}/lib/x64"
			$<TARGET_FILE_DIR:Example1>

			COMMAND ${CMAKE_COMMAND} -E copy_directory
			"${CMAKE_CURRENT_SOURCE_DIR}/${SDL2_IMAGE_PATH}/lib/x64"
			$<TARGET_FILE_DIR:Example1>

			COMMAND ${CMAKE_COMMAND} -E copy_directory
			"${CMAKE_CURRENT_SOURCE_DIR}/${SDL2_MIXER_PATH}/lib/x64"
			$<TARGET_FILE_DIR:Example1>

			COMMAND ${CMAKE_COMMAND} -E copy_directory
			"${CMAKE_CURRENT_SOURCE_DIR}/${SDL2_TTF_PATH}/lib/x64"
			$<TARGET_FILE_DIR:Example1>
	)
endif()
