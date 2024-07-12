/*
 * game/renderer/controls.h
 *
 * Copyright (c) 2024 Omar Berrow
*/

#pragma once

#include <glm/glm.hpp>

extern bool g_dbgScreenEnabled;

namespace renderer
{
    extern glm::mat4 ViewMatrix;
    extern glm::mat4 ProjectionMatrix;
    extern float g_fov;
    extern glm::vec3 g_direction;
    extern float g_speed;
    extern float g_mouseSpeed;
    void DisableControls();
    void EnableControls();
    bool ControlsEnabled();
    extern glm::vec3 g_position;
}