# game/CMakeLists.txt
#
# Copyright (c) 2024 Omar Berrow

list (APPEND game_sources
    "main.cpp" "renderer/shader.h" "renderer/shader.cpp"  "renderer/program.cpp"
    "renderer/vao.h" "renderer/vao.cpp" "renderer/mesh.h" "renderer/mesh.cpp"
    "logger.h" "logger.cpp" "file.h" "renderer/controls.h" "renderer/controls.cpp"
    "renderer/window_callbacks.h" "renderer/window_callbacks.cpp" "renderer/texture.h" "renderer/texture.cpp"
)

add_executable(game)

target_include_directories(game PUBLIC ${GAME_EXTERNAL_INCLUDES} PRIVATE ${CMAKE_SOURCE_DIR}/src/game)

find_package(OpenGL REQUIRED)
find_package(glm REQUIRED)
find_package(assimp REQUIRED)

target_link_libraries(game
    PRIVATE ${CMAKE_SOURCE_DIR}/dependencies/GLEW/lib/libGLEW.a
    PRIVATE ${CMAKE_SOURCE_DIR}/dependencies/GLFW_Lib/lib/libglfw3.a
    PRIVATE ${OPENGL_LIBRARY}
    PRIVATE glm::glm
    PRIVATE assimp::assimp
)

target_sources(game PRIVATE ${game_sources})