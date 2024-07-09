# game/CMakeLists.txt
#
# Copyright (c) 2024 Omar Berrow

list (APPEND game_sources
    "main.cpp" "renderer/shader.h" "renderer/shader.cpp"  "renderer/program.cpp"
)

add_executable(game)

target_include_directories(game PUBLIC ${GAME_EXTERNAL_INCLUDES} PRIVATE ${CMAKE_SOURCE_DIR}/src/game)

find_package(OpenGL REQUIRED)

target_link_libraries(game
    PRIVATE ${CMAKE_SOURCE_DIR}/dependencies/GLEW/lib/libGLEW.a
    PRIVATE ${CMAKE_SOURCE_DIR}/dependencies/GLFW_Lib/lib/libglfw3.a
    PRIVATE ${OPENGL_LIBRARY}
)

target_sources(game PRIVATE ${game_sources})