# dependencies/glew.cmake

# Copyright (c) 2024 Omar Berrow

include(FetchContent)

FetchContent_Declare(GLEW
	URL https://github.com/nigels-com/glew/releases/download/glew-2.2.0/glew-2.2.0.tgz
	SOURCE_DIR ${CMAKE_SOURCE_DIR}/dependencies/GLEW
)
FetchContent_MakeAvailable(GLEW)
exec_program(
	"make"
	ARGS "-C ${CMAKE_SOURCE_DIR}/dependencies/GLEW"
)
exec_program(
	"${CMAKE_COMMAND}"
	ARGS "-E copy_directory ${CMAKE_SOURCE_DIR}/dependencies/GLEW/include ${CMAKE_SOURCE_DIR}/dependencies/include"
)