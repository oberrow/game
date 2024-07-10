/*
 * game/renderer/vao.h
 *
 * Copyright (c) 2024 Omar Berrow
*/

#pragma once

#include <stddef.h>

#include <GL/glew.h>

#include <list>

namespace renderer
{
    class RenderableObject
    {
    public:
        RenderableObject() = default;
        RenderableObject(const RenderableObject&) = default;
        RenderableObject& operator=(const RenderableObject&) = default;
        RenderableObject(RenderableObject&&) = default;
        RenderableObject& operator=(RenderableObject&&) = default;

        virtual GLint Bind(class VAO& to) = 0;
        virtual GLint Render() = 0;

        virtual void SetVAAIndex(GLuint vaaIndex) { m_vaaIndex = vaaIndex; };
        virtual GLuint GetVAAIndex() const { return m_vaaIndex; };

        class VAO& GetVAO() const;

        virtual ~RenderableObject() {}
    protected:
        class VAO* m_vao = nullptr;
        bool m_initialized;
        GLuint m_vaaIndex;
        void add_to_vao();
        void remove_from_vao();
    };
    class VAO
    {
    public:
        VAO();
        VAO(const VAO&) = default;
        VAO& operator=(const VAO&) = default;
        VAO(VAO&&) = default;
        VAO& operator=(VAO&&) = default;

        GLint Bind();
        GLint Render();

        virtual ~VAO();
        friend class RenderableObject;
    private:
        bool m_initialized;
        GLuint m_vao;
        std::list<RenderableObject*> m_objs;
    };
}