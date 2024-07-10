/*
 * game/renderer/mesh.h
 *
 * Copyright (c) 2024 Omar Berrow
*/

#pragma once

#include <stddef.h>

#include <GL/glew.h>

#include <vector>

#include <renderer/vao.h>

#include <glm/ext/matrix_float4x4.hpp>

namespace renderer
{
    class Mesh final : public RenderableObject
    {
    public:
        Mesh();
        Mesh(const Mesh&) = delete;
        Mesh& operator=(const Mesh&) = delete;
        Mesh(Mesh&&) = delete;
        Mesh& operator=(Mesh&&) = delete;

        bool Load(const char* objFile, size_t size);
        bool Load(const std::vector<GLfloat>& vertices, const std::vector<GLuint> indices);

        GLint Bind(VAO& to) override;
        GLint Render() override;

        GLuint GetVBO() const { return m_vbo; }
        GLuint GetEAO() const { return m_eao; }
        const std::vector<GLfloat>& GetVertices() const { return m_vertices; }
        const std::vector<GLuint>& GetIndices() const { return m_indices; }

        virtual ~Mesh();
    private:
        std::vector<GLfloat> m_vertices{};   
        std::vector<GLuint> m_indices{};   
        size_t m_nVertices = 0;
        size_t m_nIndices = 0;
        GLuint m_vbo = 0;
        GLuint m_eao = 0;
    };
}