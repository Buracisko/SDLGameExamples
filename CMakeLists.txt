cmake_minimum_required(VERSION 3.13)

project("SDL Game Examples" VERSION 1.0 LANGUAGES CXX)

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

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/sdl2)

# Find SDL2 and its libraries
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(SDL2_ttf REQUIRED)

# Shared "engine" library
add_library(engineLib STATIC
	"src/engine/engine.h"
	"src/engine/engine.cpp"
)
# Warnings as errors. Use "--compile-no-warning-as-error" to temp disable this.
set_property(TARGET engineLib PROPERTY COMPILE_WARNING_AS_ERROR ON)
target_compile_options(engineLib PRIVATE
  $<$<CXX_COMPILER_ID:MSVC>:/W4>
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic>
)
target_include_directories(engineLib PUBLIC "src/engine")
target_link_libraries(engineLib PUBLIC SDL2::Core SDL2::Main SDL2::Image SDL2::Mixer SDL2::TTF)

# Example 1
add_executable(Example1
	"example1/src/example1.cpp"
)
target_link_libraries(Example1 PRIVATE engineLib)

# Copy libraries to output directory
if(MSVC)
	add_custom_command(
		TARGET engineLib POST_BUILD 
			COMMAND ${CMAKE_COMMAND} -E copy_directory
			"${CMAKE_CURRENT_SOURCE_DIR}/${SDL2_PATH}/lib/x64"
			$<TARGET_FILE_DIR:engineLib>

			COMMAND ${CMAKE_COMMAND} -E copy_directory
			"${CMAKE_CURRENT_SOURCE_DIR}/${SDL2_IMAGE_PATH}/lib/x64"
			$<TARGET_FILE_DIR:engineLib>

			COMMAND ${CMAKE_COMMAND} -E copy_directory
			"${CMAKE_CURRENT_SOURCE_DIR}/${SDL2_MIXER_PATH}/lib/x64"
			$<TARGET_FILE_DIR:engineLib>

			COMMAND ${CMAKE_COMMAND} -E copy_directory
			"${CMAKE_CURRENT_SOURCE_DIR}/${SDL2_TTF_PATH}/lib/x64"
			$<TARGET_FILE_DIR:engineLib>
	)
endif()
