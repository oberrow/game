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

static const float initialSpeedWalk = 0.15f;
static const float initialSpeedSprint = 0.35f;
static const float speedCapWalk = 4.f;
static const float speedCapSprint = 6.f;
static const float speedIntervalWalk = .1f;
static const float speedIntervalSprint = .3f;
static float speed = 0;
static float mouseSpeed = 0.0035f;

namespace renderer
{
    glm::mat4 ViewMatrix{ 0 };
    glm::mat4 ProjectionMatrix{ 0 };
    
    float g_fov = 60.0f;
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
        glfwSetInputMode(g_window, GLFW_STICKY_KEYS, GL_TRUE);
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
        
        static bool isCtrlPressed = false;
        static bool isEscPressed = false;
        
        switch (key)
        {
        case GLFW_KEY_W: isWPressed = action == GLFW_PRESS || action == GLFW_REPEAT; break;
        case GLFW_KEY_S: isSPressed = action == GLFW_PRESS || action == GLFW_REPEAT; break;
        case GLFW_KEY_A: isAPressed = action == GLFW_PRESS || action == GLFW_REPEAT; break;
        case GLFW_KEY_D: isDPressed = action == GLFW_PRESS || action == GLFW_REPEAT; break;
        case GLFW_KEY_ESCAPE: isEscPressed = action == GLFW_PRESS || action == GLFW_REPEAT; break;
        default: return;
        }
        bool prevCtrlStatus = isCtrlPressed;
        isCtrlPressed = (mods & GLFW_MOD_CONTROL);
        const float initialSpeed = isCtrlPressed ? initialSpeedSprint : initialSpeedWalk;
        const float speedCap = isCtrlPressed ? speedCapSprint : speedCapWalk;
        const float speedInterval = isCtrlPressed ? speedIntervalSprint : speedIntervalWalk;
        if (!isWPressed && !isSPressed && !isAPressed && !isDPressed)
            speed = initialSpeed;
        if (prevCtrlStatus != isCtrlPressed)
            if (speed > speedCapWalk)
                speed = speedCapWalk; // Slow down.
        static double lastTime = glfwGetTime();
        double currentTime = glfwGetTime();
        float deltaTime = float(currentTime - lastTime);
        if (isWPressed)
        {
            if (!enabled)
                    return;
            g_position += direction * deltaTime * speed;
            if (speed < speedCap)
                speed += speedInterval;
        }
        if (isSPressed)
        {
            if (!enabled)
                return;
            g_position -= direction * deltaTime * speed;
            if (speed < speedCap)
                speed += speedInterval;
        }
        if (isDPressed)
        {
            if (!enabled)
                return;
            g_position += right * deltaTime * speed;
            if (speed < speedCap)
                speed += speedInterval;
        }
        if (isAPressed)
        {
            if (!enabled)
                return;
            g_position -= right * deltaTime * speed;
            if (speed < speedCap)
                speed += speedInterval;
        }
        if (isEscPressed)
        {
            if (enabled)
                DisableControls();
            else
                EnableControls();
        }
        ViewMatrix = glm::lookAt(
                g_position,
                g_position+direction,
                up
            );
        lastTime = currentTime;
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
        if (up.y < 0)
            up.y = 0;
        if (up.y > 1)
            up.y = 1;
        ViewMatrix = glm::lookAt(
                        g_position,
                        g_position+direction,
                        up
                    );
    }
}