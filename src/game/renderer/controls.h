/*
 * game/renderer/controls.h
 *
 * Copyright (c) 2024 Omar Berrow
*/

#pragma once

#include <glm/glm.hpp>

namespace renderer
{
    extern glm::mat4 ViewMatrix;
    extern glm::mat4 ProjectionMatrix;
    extern float g_fov;
    void DisableControls();
    void EnableControls();
    extern glm::vec3 g_position;
}