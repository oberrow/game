/*
 * game/renderer/shader.cpp
 *
 * Copyright (c) 2024 Omar Berrow
*/

#include <stddef.h>

#include <GL/glew.h>
#include <GL/gl.h>

#include <string_view>
#include <mutex>
#include <stdexcept>

#include <renderer/shader.h>


namespace renderer
{
    Shader::Shader(ShaderType type)
        :m_program{ nullptr }
    {
        GLenum shaderType = GL_FALSE;
        switch (type) 
        {
        case ShaderType::Fragment:
            shaderType = GL_FRAGMENT_SHADER;
            break;
        case ShaderType::Geometry:
            shaderType = GL_GEOMETRY_SHADER;
            break;
        case ShaderType::Vertex:
            shaderType = GL_VERTEX_SHADER;
            break;
        default:
            throw std::runtime_error{ "Invalid shader type passed to Shader::Shader(ShaderType type).\n" };
            return;
        }
        m_shaderId = glCreateShader(shaderType);
        m_type = type;
        m_initialized = true;
    }

    GLint Shader::CompileShader(const std::string_view& code)
    {
        return this->CompileShader(code.data(), code.size());
    }
    GLint Shader::CompileShader(const char* code, size_t size)
    {
        if (!m_initialized)
            throw std::runtime_error{ "Shader is uninitialized before compile! This is a bug, please report it.\n"};
        if (m_compileSuccess)
            return GL_FALSE; // The object is already compiled; abort.
        if (!m_lock.try_lock())
            return GL_FALSE; // The object is already being compiled, we shouldn't do it at the same time.
        GLint sz = size;
        glShaderSource(m_shaderId, 1, &code, &sz);
        glCompileShader(m_shaderId);
        // Get the compile status.
        GLint status = GL_TRUE;
        glGetShaderiv(m_shaderId, GL_COMPILE_STATUS, &status);
        // Get the info log.
        GLint infoLogLength = 0;
        glGetShaderiv(m_shaderId, GL_INFO_LOG_LENGTH, &infoLogLength);
        if (!infoLogLength)
            goto no_log;
        m_compileMessages.resize(infoLogLength);
        glGetShaderInfoLog(m_shaderId, infoLogLength, NULL, m_compileMessages.data());
        no_log:
        if (status == GL_FALSE)
        {
            m_compileSuccess = false;
            return GL_FALSE;
        }
        m_compileSuccess = true;
        m_lock.unlock();
        return GL_TRUE;
    }
    GLint Shader::BindShader(Program& program)
    {
        if (!m_initialized)
            throw std::runtime_error{ "Shader is uninitialized before bind! This is a bug, please report it.\n"};
        if (!program.m_initialized)
            return GL_FALSE;
        if (program.m_linkSuccess)
            return GL_FALSE;
        if (!m_compileSuccess || m_program)
            return GL_FALSE;
        program.m_lock.lock();
        glAttachShader(program.m_programId, m_shaderId);
        program.m_attached.push_back(this);
        m_program = &program;
        program.m_lock.unlock();
        return GL_TRUE;
    }
    std::string Shader::GetCompileMessages() const
    {
        return m_compileMessages;
    }
    Shader::~Shader()
    {
        if (m_program)
        {
            m_program->m_attached.remove(this);
            glDetachShader(m_program->m_programId, m_shaderId);
        }
        glDeleteShader(m_shaderId);
    }
}