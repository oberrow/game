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

GLFWwindow* g_window;

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
    glewExperimental = true;
    printf("%s: Initializing GLEW.\n", __func__);
    GLenum err = glewInit();
    if (err != GLEW_OK)
    {
        fprintf(stderr, "%s: Could not initialize GLEW. Error: %s\n", __func__, glewGetErrorString(err));
        return 1;
    }

    printf("%s: Using GLEW %s.\n", __func__, glewGetString(GLEW_VERSION));

    static const GLfloat triangle[] = {
       -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
       0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
       0.0f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f,
    };

    renderer::Shader vertexShader{ renderer::ShaderType::Vertex };
    renderer::Shader fragmentShader{ renderer::ShaderType::Fragment };
    GLint status = vertexShader.CompileShader(""
        "#version 330 core\n"
        "layout(location = 0) in vec3 vertexPos;\n"
        "layout(location = 1) in vec3 vertexColor;\n"
        "out vec3 fragColor;\n"
        "\n"
        "void main()\n"
        "{\n"
        "   gl_Position.xyz = vertexPos;\n"
        "   gl_Position.w = 1.0;\n"
        "   fragColor = vertexColor;\n"
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
        "in vec3 fragColor;\n"
        "\n"
        "void main()"
        "{\n"
        "   color = fragColor;\n"
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
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle), triangle, GL_STATIC_DRAW);
    while (!glfwWindowShouldClose(g_window))
    {
        auto start = std::chrono::system_clock::now().time_since_epoch().count();

        glClear(GL_COLOR_BUFFER_BIT);

        // Render shit here.
        
        // Draw the triangle.
        program.Use();
        glBindBuffer(GL_ARRAY_BUFFER, vb);
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(0);

        auto end = std::chrono::system_clock::now().time_since_epoch().count();

        fprintf(stderr, "\rFPS: %f", 1.0f/(end-start)*std::chrono::system_clock::duration::period::den);

        glfwSwapBuffers(g_window);
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}