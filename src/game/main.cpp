/*
 * game/main.cpp
 *
 * Copyright (c) 2024 Omar Berrow
*/

#include <GL/glew.h>
#include <GL/gl.h>

#include <GLFW/glfw3.h>

#include <stdio.h>
#include <stdlib.h>

#include <chrono>
#include <unistd.h>

#include <renderer/shader.h>

#include <glm/ext/matrix_float4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/trigonometric.hpp>
#include <glm/ext/matrix_clip_space.hpp>

GLFWwindow* g_window;

static const GLfloat mesh[] = {
    -1.0f,-1.0f,-1.0f, // triangle 1 : begin
    -1.0f,-1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f, // triangle 1 : end
    1.0f, 1.0f,-1.0f, // triangle 2 : begin
    -1.0f,-1.0f,-1.0f,
    -1.0f, 1.0f,-1.0f, // triangle 2 : end
    1.0f,-1.0f, 1.0f,
    -1.0f,-1.0f,-1.0f,
    1.0f,-1.0f,-1.0f,
    1.0f, 1.0f,-1.0f,
    1.0f,-1.0f,-1.0f,
    -1.0f,-1.0f,-1.0f,
    -1.0f,-1.0f,-1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f,-1.0f,
    1.0f,-1.0f, 1.0f,
    -1.0f,-1.0f, 1.0f,
    -1.0f,-1.0f,-1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f,-1.0f, 1.0f,
    1.0f,-1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f,-1.0f,-1.0f,
    1.0f, 1.0f,-1.0f,
    1.0f,-1.0f,-1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f,-1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f,-1.0f,
    -1.0f, 1.0f,-1.0f,
    1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f,-1.0f,
    -1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,
    1.0f,-1.0f, 1.0f
};

int main()
{
    printf("%s: Starting GLFW.\n", __func__);

#ifdef __linux__
    glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);
#endif

    glewExperimental = true;
    if (!glfwInit())
    {
        fprintf(stderr, "%s: Could not initialize GLFW.\n", __func__);
        return 1;
    }

#ifdef __linux__
    if (glfwGetPlatform() != GLFW_PLATFORM_X11)
    {
        fprintf(stderr, "Fatal error: GLFW is not using X11 as the windowing system.\n");
        return 2;
    }
#endif

    printf("%s: Opening window.\n", __func__);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    g_window = glfwCreateWindow(1080, 780, "Game", nullptr, nullptr);
    if (!g_window)
    {
        fprintf(stderr, "%s: Could not open window.", __func__);
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(g_window);
    // glfwSwapInterval(0);
    glewExperimental = true;
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    printf("%s: Initializing GLEW.\n", __func__);
    GLenum err = glewInit();
    if (err != GLEW_OK)
    {
        fprintf(stderr, "%s: Could not initialize GLEW. Error: %s\n", __func__, glewGetErrorString(err));
        return 1;
    }

    printf("%s: Using GLEW %s.\n", __func__, glewGetString(GLEW_VERSION));

    renderer::Shader vertexShader{ renderer::ShaderType::Vertex };
    renderer::Shader fragmentShader{ renderer::ShaderType::Fragment };
    GLint status = vertexShader.CompileShader(""
        "#version 330 core\n"
        "layout(location = 0) in vec3 vertexPos;\n"
        "uniform mat4 MVP;\n"
        "out vec3 vertexColour;\n"
        "\n"
        "void main()\n"
        "{\n"
        "   gl_Position = MVP * vec4(vertexPos, 1);\n"
        "   vertexColour = vertexPos;\n"
        "}"
    );
    if (!status)
    {
        fprintf(stderr, "Vertex shader failed compile!\n%s\n", vertexShader.GetCompileMessages().data());
        glfwTerminate();
        return 1;
    }
    status = fragmentShader.CompileShader(""
        "#version 330 core\n"
        "out vec3 color;\n"
        "in vec3 vertexColour;\n"
        "\n"
        "void main()\n"
        "{\n"
        "   color = vertexColour;\n"
        "}"
    );
    
    if (!status)
    {
        fprintf(stderr, "Fragment shader failed compile!\n%s\n", fragmentShader.GetCompileMessages().data());
        glfwTerminate();
        return 1;
    }
    renderer::Program program;
    vertexShader.BindShader(program);
    fragmentShader.BindShader(program);
    program.Link();
    GLuint vao = 0;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    GLuint vb = 0;
    glGenBuffers(1, &vb);
    glBindBuffer(GL_ARRAY_BUFFER, vb);
    glBufferData(GL_ARRAY_BUFFER, sizeof(mesh), mesh, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, 0, (void*)0);
    glm::mat4 view = glm::lookAt(
        glm::vec3(5,3,3),
        glm::vec3(0,0,0),
        glm::vec3(0,1,0)
    );
    glm::mat4 projection = glm::perspective(
        glm::radians(45.0f),
        1080.0f / 780.0f,
        0.1f,
        100.0f
    );
    glm::mat4 mvp = projection*view*glm::mat4(1.0f);
    program.Use();
    GLuint MatrixID = program.GetUniformLocation("MVP");
    while (!glfwWindowShouldClose(g_window))
    {
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

        auto start = std::chrono::system_clock::now().time_since_epoch().count();

        // Render shit here.
        
        // Draw the triangle.
        program.Use();
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);
        glBindBuffer(GL_ARRAY_BUFFER, vb);
        glDrawArrays(GL_TRIANGLES, 0, 12*3);
        // glDisableVertexAttribArray(1);
        // glDisableVertexAttribArray(0);

        auto end = std::chrono::system_clock::now().time_since_epoch().count();

        fprintf(stderr, "\rFPS: %f", 1.0f/(end-start)*std::chrono::system_clock::duration::period::den);

        glfwSwapBuffers(g_window);
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}