/*
 * game/renderer/shader.h
 *
 * Copyright (c) 2024 Omar Berrow
*/

#pragma once

#include <stddef.h>

#include <GL/glew.h>

#include <string_view>
#include <string>
#include <mutex>
#include <list>

namespace renderer
{
    enum class ShaderType
    {
        None, Vertex, Fragment, Geometry,
        MaxValue = Geometry
    };

    // Cannot be copied.
    // Cannot be moved.
    class Program final
    {
    public:
        Program();
        Program(const Program&) = delete;
        Program& operator=(const Program&) = delete;
        Program(Program&&) = delete;
        Program& operator=(Program&&) = delete;

        GLint Link();
        GLint Use();

        GLuint GetUniformLocation(const char* uniformName);

        std::string GetLinkMessages() const;

        ~Program();
        friend class Shader;
    private:
        bool m_initialized;
        GLuint m_programId;
        std::mutex m_lock;
        std::list<class Shader*> m_attached;
        std::string m_linkMessages;
        bool m_linkSuccess;
    };
    // Cannot be copied.
    // Cannot be moved.
    class Shader final
    {
    public:
        Shader() = delete;
        Shader(ShaderType type);
        Shader(const Shader&) = delete;
        Shader& operator=(const Shader&) = delete;
        Shader(Shader&&) = delete;
        Shader& operator=(Shader&&) = delete;
        
        GLint CompileShader(const std::string_view& code);
        GLint CompileShader(const char* code, size_t size);

        GLint BindShader(Program& program);

        std::string GetCompileMessages() const;

        ~Shader();
        friend class Program;
    private:
        bool m_initialized = false;
        GLuint m_shaderId = 0;
        ShaderType m_type = ShaderType::None;
        std::mutex m_lock{};
        std::string m_compileMessages{};
        bool m_compileSuccess = false;
        Program* m_program = nullptr;
    };
}