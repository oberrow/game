/*
 * game/renderer/vao.cpp
 *
 * Copyright (c) 2024 Omar Berrow
*/

#include "renderer/shader.h"
#include <stddef.h>

#include <GL/glew.h>

#include <glm/ext/matrix_float4x4.hpp>

#include <renderer/vao.h>

#include <cassert>
#include <stdexcept>

namespace renderer
{
    VAO& RenderableObject::GetVAO() const
    {
        if (!m_vao)
            throw std::runtime_error("Object is not bound to anything.");
        return *m_vao;
    }

    VAO::VAO()
    {
        m_vao = 0;
        glCreateVertexArrays(1, &m_vao);
        m_initialized = true;
    }
    GLint VAO::Bind()
    {
        if (!m_initialized)
            return GL_FALSE;
        glBindVertexArray(m_vao);
        return GL_TRUE;
    }
    GLint VAO::Render()
    {
        if (!m_initialized)
            return GL_FALSE;
        if (Bind() == GL_FALSE)
            return GL_FALSE;
        for (auto& i : m_objs)
        {
            glEnableVertexAttribArray(i->m_vaaIndex);
            i->Render();
        }
        for (auto& i : m_objs)
            glDisableVertexAttribArray(i->m_vaaIndex);
        return GL_TRUE;
    }
    VAO::~VAO()
    {
        if (m_initialized)
        {
            m_objs.clear();
            glDeleteVertexArrays(1, &m_vao);
        }
    }
    void RenderableObject::add_to_vao()
    {
        assert(m_vao);
        m_vao->m_objs.push_back(this);
    }
    void RenderableObject::remove_from_vao()
    {
        assert(m_vao);
        m_vao->m_objs.remove(this);
    }
}