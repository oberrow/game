/*
 * game/main.cpp
 *
 * Copyright (c) 2024 Omar Berrow
*/

#include "renderer/texture.h"
#include <GL/glew.h>
#include <GL/gl.h>

#include <GLFW/glfw3.h>

#ifdef DEBUG_SCREEN
// NOTE(oberrow): Might only work on my system.
#   include <imgui/imgui.h>
#   include <imgui/backends/imgui_impl_glfw.h>
#   include <imgui/backends/imgui_impl_opengl3.h>
#endif

#include <stdlib.h>

#include <chrono>
#include <string>

#include <renderer/shader.h>
#include <renderer/vao.h>
#include <renderer/mesh.h>
#include <renderer/controls.h>
#include <renderer/window_callbacks.h>

#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/trigonometric.hpp>
#include <glm/ext/matrix_clip_space.hpp>

#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include <file.h>
#include <logger.h>

GLFWwindow* g_window;

int main(int argc, const char** argv)
{
    if (argc >= 2)
    {
        logger::log_level logLevel = (logger::log_level)std::atoi(argv[1]);
        logger::SetLogLevel(logLevel);
    }
    else {
        logger::SetLogLevel(logger::log_level::Log);
    }
    logger::Log("Initializing renderer.\n");
    logger::Debug("%s: Starting GLFW.\n", __func__);

#ifdef __linux__
    glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);
#endif

    glewExperimental = true;
    if (!glfwInit())
    {
        logger::Error("%s: Could not initialize GLFW.\n", __func__);
        return 1;
    }

#ifdef __linux__
    if (glfwGetPlatform() != GLFW_PLATFORM_X11)
    {
        logger::Error("Fatal error: GLFW is not using X11 as the windowing system.\n");
        return 2;
    }
#endif

    logger::Debug("%s: Opening window.\n", __func__);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    g_window = glfwCreateWindow(1024, 768, "Game", nullptr, nullptr);
    if (!g_window)
    {
        logger::Error("%s: Could not open window.", __func__);
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(g_window);
    // glfwSwapInterval(0);
    glewExperimental = true;
    glfwSwapInterval(1);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glDepthFunc(GL_LESS);
    // glEnable(GL_BLEND);
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

#ifdef DEBUG_SCREEN
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    ImGui_ImplGlfw_InitForOpenGL(g_window, true);

    ImGui::StyleColorsDark();

    ImGui_ImplOpenGL3_Init("#version 330 core");
#endif

    logger::Debug("%s: Initializing GLEW.\n", __func__);
    GLenum err = glewInit();
    if (err != GLEW_OK)
    {
        logger::Error("%s: Could not initialize GLEW. Error: %s\n", __func__, glewGetErrorString(err));
        return 1;
    }

    logger::Debug("%s: Using GLEW %s.\n", __func__, glewGetString(GLEW_VERSION));

    renderer::Shader vertexShader{ renderer::ShaderType::Vertex };
    renderer::Shader fragmentShader{ renderer::ShaderType::Fragment };
    GLint status = vertexShader.CompileShader(""
        "#version 330 core\n"
        "layout(location = 0) in vec3 vertexPos;\n"
        "layout(location = 1) in vec2 vertexUV;\n"
        "uniform mat4 MVP;\n"
        "out vec2 uv;\n"
        "\n"
        "void main()\n"
        "{\n"
        "   gl_Position = MVP * vec4(vertexPos, 1.0);\n"
        "   uv = vec2(vertexUV.x, 1.0-vertexUV.y);\n"
        "}"
    );
    if (!status)
    {
        logger::Error("Vertex shader failed compile!\n%s\n", vertexShader.GetCompileMessages().data());
        glfwTerminate();
        return 1;
    }
    status = fragmentShader.CompileShader(""
        "#version 330 core\n"
        "out vec4 color;\n"
        "in vec2 uv;\n"
        "uniform sampler2D textureSampler;\n"
        "\n"
        "void main()\n"
        "{\n"
        "   color = vec4(texture(textureSampler, uv));\n"
        "}"
    );

    if (!status)
    {
        logger::Error("Fragment shader failed compile!\n%s\n", fragmentShader.GetCompileMessages().data());
        glfwTerminate();
        return 1;
    }
    renderer::Program program;
    vertexShader.BindShader(program);
    fragmentShader.BindShader(program);
    program.Link();

    renderer::VAO vao;
    renderer::Mesh meshObj;
    renderer::Texture textureObj;
    std::string dat = "";
    std::vector<uint8_t> texture;
    if (!utility::LoadFile("cube.obj", dat))
    {
        logger::Error("Could not find file %s.", "cube.obj");
        glfwTerminate();
        return 1;
    }
    if (!utility::LoadFile("cube.bmp", texture))
    {
        logger::Error("Could not find file %s.", "cube.bmp");
        glfwTerminate();
        return 1;
    }
    std::vector<GLfloat> vertices, textureCoords, normals;
    std::vector<GLuint> indices;
    if (!renderer::LoadMesh(dat.c_str(), dat.length(), vertices, indices, textureCoords, normals))
    {
        glfwTerminate();
        return 1;
    }
    meshObj.SetVAAIndex(0);
    meshObj.Load(vertices, indices);
    textureObj.Load(texture.data(), texture.size(), textureCoords);
    textureObj.SetVAAIndex(1);
    textureObj.Bind(vao);
    meshObj.Bind(vao);
    program.Use();
    GLuint MatrixID = program.GetUniformLocation("MVP");
    GLuint TextureSamplerId = program.GetUniformLocation("textureSampler");
    textureObj.SetTextureSamplerUniform(TextureSamplerId);
    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
    glm::quat rotation = glm::quat(glm::vec3(90, 45, 0));
    glm::mat4 rotationMatrix = glm::toMat4(rotation);
    glm::mat4 model1{};
    glm::mat4 model2{};
    model1 = glm::translate(glm::mat4(1.f), glm::vec3(-0,0,0))*rotationMatrix*glm::mat4(1.f);
    model2 = glm::translate(glm::mat4(1.f), glm::vec3( 5,0,0))*glm::mat4(1.f);

    renderer::EnableControls();
    logger::Log("Initialized renderer.\n");
    while (!glfwWindowShouldClose(g_window))
    {
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

        glm::mat4 mvp1 = renderer::ProjectionMatrix*renderer::ViewMatrix*model1;
        glm::mat4 mvp2 = renderer::ProjectionMatrix*renderer::ViewMatrix*model2;

        auto start = std::chrono::system_clock::now().time_since_epoch().count();
    
        // Render shit here.

        program.Use();

        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp1[0][0]);
        vao.Render();
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp2[0][0]);
        vao.Render();

        auto end = std::chrono::system_clock::now().time_since_epoch().count();

#ifdef DEBUG_SCREEN
        // Render debug screen shit here.        
        if (g_dbgScreenEnabled)
        {
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            ImGui::Begin("Debug screen", &g_dbgScreenEnabled);
            ImGui::Text("FPS: %f", 1.0f/(end-start)*std::chrono::system_clock::duration::period::den);
            ImGui::Text("XYZ: %f,%f,%f", renderer::g_position.x,renderer::g_position.y,renderer::g_position.z);
            ImGui::Text("Facing: %s,%s,%s", 
                renderer::g_direction.x < 0 ? "-x" : renderer::g_direction.x == 0 ? "x" : "+x",
                renderer::g_direction.y < 0 ? "-y" : renderer::g_direction.y == 0 ? "y" : "+y",
                renderer::g_direction.z < 0 ? "-z" : renderer::g_direction.z == 0 ? "z" : "+z"
            );
            ImGui::Text("Speed: %f", renderer::g_speed);
            ImGui::SliderFloat("FoV", &renderer::g_fov, 30, 120);
            float sensivity = renderer::g_mouseSpeed*10000; 
            if (ImGui::SliderFloat("Sensivity", &sensivity, 0, 100))
                renderer::g_mouseSpeed = sensivity/10000;
            bool value = renderer::ControlsEnabled();
            if (ImGui::Checkbox("Enable input", &value))
                value ? renderer::EnableControls() : renderer::DisableControls();
            if (ImGui::Button("Stop"))
                glfwSetWindowShouldClose(g_window, 1);
            ImGui::End();
        
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }
#endif

        glfwSwapBuffers(g_window);
        glfwPollEvents();
    }

#ifdef DEBUG_SCREEN
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
#endif

    glfwTerminate();

    return 0;
}