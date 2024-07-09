/*
 * game/renderer/shader.cpp
 *
 * Copyright (c) 2024 Omar Berrow
*/

#include "renderer/shader.h"
#include <stddef.h>

#include <GL/glew.h>
#include <GL/gl.h>

#include <string_view>
#include <string>
#include <mutex>
#include <list>

namespace renderer
{
    Program::Program()
    {
        m_programId = glCreateProgram();
        m_initialized = true;
    }
    GLint Program::Link()
    {
        if (!m_initialized)
            throw std::runtime_error{ "Program is uninitialized before link! This is a bug, please report it.\n"};
        if (!m_attached.size())
            return GL_FALSE; // Nothing to link.
        m_lock.lock();
        glLinkProgram(m_programId);
        GLint result = GL_TRUE;
        glGetProgramiv(m_programId, GL_LINK_STATUS, &result);
        GLint infoLogLength = 0;
        glGetProgramiv(m_programId, GL_INFO_LOG_LENGTH, &infoLogLength);
        if (!infoLogLength)
            goto nolog;
        m_linkMessages.resize(infoLogLength);
        glGetProgramInfoLog(m_programId, infoLogLength, NULL, m_linkMessages.data());
        nolog:
        if (result == GL_FALSE)
        {
            m_linkSuccess = false;
            return GL_FALSE; 
        }
        m_linkSuccess = true;
        // Dereference all attached objects.
        for (auto i : m_attached)
        {
            i->m_program = nullptr;
            glDetachShader(m_programId, i->m_shaderId);
        }
        m_attached.clear(); // Remove each shader.
        m_lock.unlock();
        return GL_TRUE;
    }
    GLint Program::Use()
    {
        if (!m_initialized)
            throw std::runtime_error{ "Program is uninitialized before call to Use()! This is a bug, please report it.\n"};
        if (!m_linkSuccess)
            return GL_FALSE;
        glUseProgram(m_programId);
        return GL_TRUE;
    }
    Program::~Program()
    {
        if (!m_linkSuccess)
        {
            for (auto i : m_attached)
            {
                i->m_program = nullptr;
                glDetachShader(m_programId, i->m_shaderId);
            }
            m_attached.clear();
        }
        glDeleteProgram(m_programId);
    }
}