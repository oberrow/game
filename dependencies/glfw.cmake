# dependencies/glfw.cmake

# Copyright (c) 2024 Omar Berrow

include(FetchContent)

file(MAKE_DIRECTORY "dependencies/include/GLFW")
FetchContent_Declare(GLFW
	GIT_REPOSITORY https://github.com/glfw/glfw.git
	GIT_TAG 7b6aead9fb88b3623e3b3725ebb42670cbe4c579
	SOURCE_DIR ${CMAKE_SOURCE_DIR}/dependencies/GLFW
	BINARY_DIR ${CMAKE_SOURCE_DIR}/dependencies/GLFW_Build
)
FetchContent_MakeAvailable(GLFW)
exec_program(
	"${CMAKE_COMMAND}"
	ARGS "-B ${CMAKE_SOURCE_DIR}/dependencies/GLFW_Build ${CMAKE_SOURCE_DIR}/dependencies/GLFW -DGLFW_BUILD_WAYLAND=false"
)
exec_program(
	"${CMAKE_COMMAND}"
	ARGS "--build ${CMAKE_SOURCE_DIR}/dependencies/GLFW_Build -t glfw"
)
exec_program(
	"${CMAKE_COMMAND}"
	ARGS "--install ${CMAKE_SOURCE_DIR}/dependencies/GLFW_Build --prefix ${CMAKE_SOURCE_DIR}/dependencies/GLFW_Lib"
)
file (COPY "${CMAKE_SOURCE_DIR}/dependencies/GLFW/include/" DESTINATION "${CMAKE_SOURCE_DIR}/dependencies/include/")