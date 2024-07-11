/*
 * game/renderer/Texture.h
 *
 * Copyright (c) 2024 Omar Berrow
*/

#pragma once

#include <stddef.h>

#include <GL/glew.h>

#include <vector>

#include <renderer/vao.h>

namespace renderer
{
    class Texture final : public RenderableObject
    {
    public:
        Texture();
        Texture(const Texture&) = delete;
        Texture& operator=(const Texture&) = delete;
        Texture(Texture&&) = delete;
        Texture& operator=(Texture&&) = delete;

        bool Load(const void* image, size_t szImage, const std::vector<GLfloat>& textureCoordinates);

        GLint Bind(VAO& to) override;
        GLint Render() override;

        GLuint GetVBO() const { return m_vbo; }

        void SetTextureSamplerUniform(GLuint to) { m_textureSamplerUniform = to; }

        virtual ~Texture();
    private:
        std::vector<GLfloat> m_textureCoordinates{}; 
        size_t m_nCoords = 0;
        GLuint m_vbo = 0;
        GLuint m_textureObject = 0;
        GLuint m_textureSamplerUniform = 0;
        const void* m_image = nullptr;
        size_t m_szImage = 0;
    };
}