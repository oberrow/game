/*
 * game/renderer/controls.cpp
 * Not mine.
 * Taken from https://github.com/opengl-tutorials/ogl/blob/master/common/controls.cpp, with some changes.
*/

#include "logger.h"
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <renderer/controls.h>

extern GLFWwindow* g_window;


static float horizontalAngle = 0.0f;
static float verticalAngle = 0.0f;
static float initialFoV = 45.0f;

static float speed = 3.0f;
static float mouseSpeed = 0.005f;

namespace renderer
{
    glm::mat4 ViewMatrix;
    glm::mat4 ProjectionMatrix;
    
    glm::vec3 g_position = glm::vec3( 0, 3, 0 ); 
    void CalculateNewMatrices()
    {
        static double lastTime = glfwGetTime();

        double currentTime = glfwGetTime();
        float deltaTime = float(currentTime - lastTime);

        double xpos, ypos;
        glfwGetCursorPos(g_window, &xpos, &ypos);

        int screenWidth = 0;
        int screenHeight = 0;
        glfwGetWindowSize(g_window, &screenWidth, &screenHeight);

        glfwSetCursorPos(g_window, screenWidth/2.f, screenHeight/2.f);

        horizontalAngle += mouseSpeed * float(screenWidth/2.f - xpos );
        verticalAngle   += mouseSpeed * float(screenHeight/2.f - ypos );

        glm::vec3 direction(
            cos(verticalAngle) * sin(horizontalAngle), 
            sin(verticalAngle),
            cos(verticalAngle) * cos(horizontalAngle)
        );
        
        glm::vec3 right = glm::vec3(
            sin(horizontalAngle - 3.14f/2.0f), 
            0,
            cos(horizontalAngle - 3.14f/2.0f)
        );
        
        glm::vec3 up = glm::cross( right, direction );

        bool posChanged = false;

        if (glfwGetKey(g_window, GLFW_KEY_W ) == GLFW_PRESS)
            { posChanged = true; g_position += direction * deltaTime * speed;}
        if (glfwGetKey(g_window, GLFW_KEY_S ) == GLFW_PRESS)
            { posChanged = true; g_position -= direction * deltaTime * speed; }
        if (glfwGetKey(g_window, GLFW_KEY_D ) == GLFW_PRESS)
            { posChanged = true; g_position += right * deltaTime * speed; }
        if (glfwGetKey(g_window, GLFW_KEY_A ) == GLFW_PRESS)
            { posChanged = true; g_position -= right * deltaTime * speed; }
        if (posChanged)
            logger::Debug("Position changed. New position: %f, %f, %f.\n", g_position.x, g_position.y, g_position.z);

        float FoV = initialFoV;

        ProjectionMatrix = glm::perspective(glm::radians(FoV), (float)screenWidth/(float)screenHeight, 0.1f, 100.0f);
        ViewMatrix = glm::lookAt(
                        g_position,
                        g_position+direction,
                        up
                    );

        lastTime = currentTime;
    }
}