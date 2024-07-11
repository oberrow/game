/*
 * game/renderer/controls.cpp
 * Not mine.
 * Taken from https://github.com/opengl-tutorials/ogl/blob/master/common/controls.cpp, with some changes.
*/

#include <logger.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <renderer/controls.h>
#include <renderer/window_callbacks.h>

extern GLFWwindow* g_window;


static float horizontalAngle = 0.f;
static float verticalAngle = 0.0f;

static float speed = 0.5f;
static const float speedCap = 3.f;
static const float speedInterval = .2f;
static float mouseSpeed = 0.0035f;

namespace renderer
{
    glm::mat4 ViewMatrix{ 0 };
    glm::mat4 ProjectionMatrix{ 0 };
    
    float g_fov = 45.0f;
    glm::vec3 g_position = glm::vec3( 0, 3, 0 ); 
    static bool enabled = true;
    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
    void DisableControls()
    {
        enabled = false;
        glfwSetInputMode(g_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
    void EnableControls()
    {
        enabled = true;
        glfwSetCursorPosCallback(g_window, cursor_position_callback);
        glfwSetKeyCallback(g_window, key_callback);
        glfwSetInputMode(g_window, GLFW_STICKY_KEYS, GL_FALSE);
        glfwSetInputMode(g_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwSetWindowFocusCallback(g_window, renderer::OnFocusCallback);
        glfwSetWindowSizeCallback(g_window, renderer::OnResizeCallback);
        int screenWidth = 0;
        int screenHeight = 0;
        glfwGetWindowSize(g_window, &screenWidth, &screenHeight);
        glfwSetCursorPos(g_window, screenWidth/2.0, screenHeight/2.0);
        cursor_position_callback(g_window, screenWidth/2.0, screenHeight/2.0);
        ProjectionMatrix = glm::perspective(glm::radians(g_fov), (float)screenWidth/(float)screenHeight, 0.1f, 100.0f);
        glfwShowWindow(g_window);
    }
    static glm::vec3 direction{0,0,0};
    static glm::vec3 right{0,0,0};
    static glm::vec3 up{0,0,0};
    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        static bool isWPressed = false;
        static bool isSPressed = false;
        static bool isAPressed = false;
        static bool isDPressed = false;
        if (action == GLFW_PRESS || action == GLFW_REPEAT)
        {
            static double lastTime = glfwGetTime();
            if (!enabled)
                return;

            double currentTime = glfwGetTime();
            float deltaTime = float(currentTime - lastTime);
            switch(key)
            {
                case GLFW_KEY_W:
                {
                    isWPressed = true;
                    g_position += direction * deltaTime * speed;
                    if (speed < speedCap)
                        speed += speedInterval;
                    // logger::Debug("Changed position to %lf,%lf,%lf.\n", g_position.x, g_position.y, g_position.z);
                    break;
                }
                case GLFW_KEY_S:
                {
                    isSPressed = true;
                    g_position -= direction * deltaTime * speed;
                    if (speed < speedCap)
                        speed += speedInterval;
                    // logger::Debug("Changed position to %lf,%lf,%lf.\n", g_position.x, g_position.y, g_position.z);
                    break;
                }
                case GLFW_KEY_D:
                {
                    isDPressed = true;
                    g_position += right * deltaTime * speed;
                    if (speed < speedCap)
                        speed += speedInterval;
                    // logger::Debug("Changed position to %lf,%lf,%lf.\n", g_position.x, g_position.y, g_position.z);
                    break;
                }
                case GLFW_KEY_A:
                {
                    isAPressed = true;
                    g_position -= right * deltaTime * speed;
                    if (speed < speedCap)
                        speed += speedInterval;
                    // logger::Debug("Changed position to %lf,%lf,%lf.\n", g_position.x, g_position.y, g_position.z);
                    break;
                }
                case GLFW_KEY_ESCAPE:
                {
                    if (enabled)
                        DisableControls();
                    else
                        EnableControls();
                    break;
                }
                default: break;
            }
            ViewMatrix = glm::lookAt(
                    g_position,
                    g_position+direction,
                    up
                );
            lastTime = currentTime;
        }
        else
        {
            bool isMovementKey = true;
            switch(key)
            {
                case GLFW_KEY_W:
                {
                    isWPressed = false;
                    break;
                }
                case GLFW_KEY_S:
                {
                    isSPressed = false;
                    break;
                }
                case GLFW_KEY_D:
                {
                    isDPressed = false;
                    break;
                }
                case GLFW_KEY_A:
                {
                    isAPressed = false;
                    break;
                }
                default: isMovementKey = false; break;
            }
            if (isMovementKey && (
                !isWPressed && !isSPressed &&
                !isAPressed && !isDPressed
                )
            )
                speed = 0.5f;
        }
    }
    static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
    {
        if (!enabled)
                return;
        // logger::Debug("Cursor moved to %lf,%lf.\n", xpos, ypos);
        int screenWidth = 0;
        int screenHeight = 0;
        glfwGetWindowSize(g_window, &screenWidth, &screenHeight);

        glfwSetCursorPos(g_window, screenWidth/2.f, screenHeight/2.f);

        horizontalAngle += mouseSpeed * float(screenWidth/2.f - xpos );
        verticalAngle   += mouseSpeed * float(screenHeight/2.f - ypos );

        direction = glm::vec3(
            cos(verticalAngle) * sin(horizontalAngle), 
            sin(verticalAngle),
            cos(verticalAngle) * cos(horizontalAngle)
        );
        
        right = glm::vec3(
            sin(horizontalAngle - 3.14f/2.0f), 
            0,
            cos(horizontalAngle - 3.14f/2.0f)
        );
        up = glm::cross( right, direction );
        ViewMatrix = glm::lookAt(
                        g_position,
                        g_position+direction,
                        up
                    );
    }
}