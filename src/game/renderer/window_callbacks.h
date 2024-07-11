/*
 * game/renderer/window_callbacks.h
 *
 * Copyright (c) 2024 Omar Berrow
*/

#pragma once

#include <GLFW/glfw3.h>

extern GLFWwindow* g_window;

namespace renderer
{
    void OnFocusCallback(GLFWwindow* window, int focused);
    void OnResizeCallback(GLFWwindow* window, int width, int height);
}