/*
 * game/renderer/mesh.cpp
 *
 * Copyright (c) 2024 Omar Berrow
*/

#include "logger.h"
#include <assimp/mesh.h>
#include <stddef.h>

#include <GL/glew.h>

#include <renderer/vao.h>
#include <renderer/mesh.h>

#include <sstream>
#include <string>
#include <map>

#include <string.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace renderer
{
    Mesh::Mesh()
    {
        m_vbo = 0;
        GLuint buffers[2] = {0,0};
        glGenBuffers(2, buffers);
        m_vbo = buffers[0];
        m_eao = buffers[1];
        m_initialized = true;
    }
    bool Mesh::Load(const std::vector<GLfloat>& vertices, const std::vector<GLuint> indices)
    {
        m_vertices = vertices;
        m_nVertices = vertices.size();
        m_indices = indices;
        m_nIndices = indices.size();
        return true;
    }
    GLint Mesh::Bind(VAO& to)
    {
        if (!m_initialized)
            return GL_FALSE;
        if (m_vao)
            return GL_FALSE;
        to.Bind();
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, m_vertices.size()*sizeof(GLfloat), m_vertices.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_eao);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size()*sizeof(GLuint), m_indices.data(), GL_STATIC_DRAW);
        m_vao = &to;
        // m_vertices.clear();
        // m_indices.clear();
        add_to_vao();
        return GL_TRUE;
    }
    GLint Mesh::Render()
    {
        if (!m_initialized)
            return GL_FALSE;
        if (!m_vao)
            return GL_FALSE;
        // m_vao->Bind();
        glEnableVertexAttribArray(m_vaaIndex);
        // Send the vertices to the shader.
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glVertexAttribPointer(m_vaaIndex, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        // Render the mesh
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_eao);
        glDrawElements(GL_TRIANGLES, m_nVertices, GL_UNSIGNED_INT, nullptr);
        // glDrawArrays(GL_TRIANGLES, 0, m_nVertices*3);
        glDisableVertexAttribArray(m_vaaIndex);
        return GL_TRUE;
    }
    Mesh::~Mesh() 
    {
        if (m_initialized)
        {
            if (m_vao)
                remove_from_vao();
            glDeleteBuffers(1, &m_vbo);
        }
    }

    bool LoadMesh(
        const char* objFile, size_t size, 
        std::vector<GLfloat> &vertices, std::vector<GLuint> &indices,
        std::vector<GLfloat> &textureCoords,
        std::vector<GLfloat> &normals
    )
    {
        Assimp::Importer importer;
        auto scene = importer.ReadFileFromMemory(
            objFile, size,
             aiProcess_JoinIdenticalVertices |
                     aiProcess_Triangulate
                     );
        if (!scene)
        {
            logger::Error("%s: Error importing file.\nError message: %s.\n", __func__, importer.GetErrorString());
            return false;
        }
        if (!scene->HasMeshes())
        {
            logger::Error("%s: Error importing file.\nError message: %s.\n", __func__, "No meshes in file!");
            return false;
        }        
        const aiMesh* mesh = scene->mMeshes[0];
        vertices.reserve(mesh->mNumVertices*3);
        for (size_t i = 0; i < mesh->mNumVertices; i++)
        {
            auto vec = mesh->mVertices[i];
            vertices.push_back(vec.x);
            vertices.push_back(vec.y);
            vertices.push_back(vec.z);  
        }
        indices.reserve(3 * mesh->mNumFaces);
	    for (size_t i = 0; i < mesh->mNumFaces; i++)
        {
            // It should be fine to assume that the model only has triangles, as we told AssImp to
            // trianglate the model.
	    	indices.push_back(mesh->mFaces[i].mIndices[0]);
	    	indices.push_back(mesh->mFaces[i].mIndices[1]);
	    	indices.push_back(mesh->mFaces[i].mIndices[2]);
        }
        textureCoords.reserve(2 * mesh->mNumVertices);
	    for (size_t i = 0; i < mesh->mNumVertices; i++)
        {
	    	textureCoords.push_back(mesh->mTextureCoords[0][i].x);
	    	textureCoords.push_back(mesh->mTextureCoords[0][i].y);
        }
        normals.reserve(3 * mesh->mNumVertices);
	    for (size_t i = 0; i < mesh->mNumVertices; i++)
        {
	    	normals.push_back(mesh->mNormals[i].x);
	    	normals.push_back(mesh->mNormals[i].y);
	    	normals.push_back(mesh->mNormals[i].z);
        }
        return true;
    }
}