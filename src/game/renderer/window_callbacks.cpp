/*
 * game/renderer/window_callbacks.cpp
 *
 * Copyright (c) 2024 Omar Berrow
*/

#include <GL/gl.h>
#include <GLFW/glfw3.h>

#include <renderer/window_callbacks.h>
#include <renderer/controls.h>

#include <glm/ext/matrix_clip_space.hpp>

namespace renderer
{
    void OnFocusCallback(GLFWwindow* window, int focused)
    {
        if (focused)
            EnableControls();
        else
            DisableControls();
        // Basically all we need to do (for now).
        // TODO: Pause the game.   
    }
    void OnResizeCallback(GLFWwindow* window, int width, int height)
    {
        glViewport(0,0, width, height);
        // Recalculate the projection matrix.
        ProjectionMatrix = glm::perspective(glm::radians(g_fov), (float)width/(float)height, 0.1f, 100.0f);
    }
}