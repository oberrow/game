# CMakeLists.txt
#
# Copyright (c) 2024 Omar Berrow

cmake_minimum_required(VERSION 3.12)

cmake_policy(SET CMP0135 NEW)

# Download dependencies.
# NOTE: As dependencies are added, please add them to the .gitignore file.
# If a dependency cmake file is included, and GAME_REFRESH_DEPENDENCIES is false, the cmake file shall not download or make any file operations, and only set any cache variables related 
# to the dependency (eg. limine_install, zydis_c, uacpi_cmake_file, etc.)
if (EXISTS ${CMAKE_SOURCE_DIR}/dependencies/needs_download.cmake)
	include (dependencies/needs_download.cmake)
else()
	set (GAME_REFRESH_DEPENDENCIES true)
endif()

set (GAME_EXTERNAL_INCLUDES "${CMAKE_SOURCE_DIR}/dependencies/include")
if (GAME_REFRESH_DEPENDENCIES)
	file(MAKE_DIRECTORY "dependencies/include")
	include("dependencies/glfw.cmake")
	include("dependencies/glew.cmake")
	file (WRITE "${CMAKE_SOURCE_DIR}/dependencies/needs_download.cmake"
		"# This file is auto-generated.\n# Set this variable to true or delete this file to refresh all dependencies.\nset (GAME_REFRESH_DEPENDENCIES false)"
	)
else()
    
endif()

if (NOT DEFINED OUTPUT_DIR)
	set (OUTPUT_DIR ${CMAKE_SOURCE_DIR}/out)
endif()

project (game CXX)

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR})

execute_process(COMMAND
  "${GIT_EXECUTABLE}" describe --match=NeVeRmAtCh --always --abbrev=40 --dirty
  WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
  OUTPUT_VARIABLE GIT_SHA1
  ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)

# the date of the commit
execute_process(COMMAND
  "${GIT_EXECUTABLE}" log -1 --format=%ad --date=local
  WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
  OUTPUT_VARIABLE GIT_DATE
  ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)

set_property(GLOBAL PROPERTY C_STANDARD 20
                    PROPERTY C_STANDARD_REQUIRED true)

add_compile_definitions (GIT_SHA1="${GIT_SHA1}" GIT_DATE="${GIT_DATE}")

add_subdirectory("src/game")