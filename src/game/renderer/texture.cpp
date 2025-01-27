/*
 * game/renderer/Texture.h
 *
 * Copyright (c) 2024 Omar Berrow
*/

#include <cstdint>
#include <stddef.h>
#include <string.h>

#include <GL/glew.h>
#include <GL/gl.h>

#include <vector>

#include <renderer/vao.h>
#include <renderer/texture.h>

#define STB_IMAGE_IMPLEMENTATION 1
#include <external/stb_image.h>

namespace renderer
{
    Texture::Texture()
    {
        m_vbo = 0;
        GLuint buffers[1] = { 0 };
        glGenBuffers(1, buffers);
        m_vbo = buffers[0];
        glGenTextures(1, &m_textureObject);
        m_initialized = true;
    }
    bool Texture::Load(const void* image, size_t szImage, const std::vector<GLfloat>& textureCoordinates)
    {
        if (!image || szImage < 124)
            return false;
        m_textureCoordinates = textureCoordinates;
        m_nCoords = textureCoordinates.size();
        uint8_t* img = (uint8_t*)image;
        m_isDDSImage = true;
        if (memcmp(img, "DDS ", 4) != 0)
        {
            m_isDDSImage = false;
            if (!stbi_info_from_memory(img, szImage, nullptr, nullptr, nullptr))
                return false; // Unrecognized format.
        }
        m_image = new uint8_t[szImage];
        memcpy((void*)m_image, image, szImage);
        m_szImage = szImage;
        return true;
    }
    #define FOURCC_DXT1 0x31545844
    #define FOURCC_DXT3 0x33545844
    #define FOURCC_DXT5 0x35545844
    
    GLint Texture::BindDDSTexture()
    {
        glBindTexture(GL_TEXTURE_2D, m_textureObject);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        uint8_t* header = (uint8_t*)m_image;
        header += 4;
        unsigned int height      = *(unsigned int*)&(header[ 8]);
	    unsigned int width	     = *(unsigned int*)&(header[12]);
	    unsigned int linearSize	 = *(unsigned int*)&(header[16]);
	    unsigned int mipMapCount = *(unsigned int*)&(header[24]);
	    unsigned int fourCC      = *(unsigned int*)&(header[80]);
        unsigned int components  = (fourCC == FOURCC_DXT1) ? 3 : 4; 
	    unsigned int format;
	    switch(fourCC) 
	    { 
	    case FOURCC_DXT1: 
	    	format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT; 
	    	break; 
	    case FOURCC_DXT3: 
	    	format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT; 
	    	break; 
	    case FOURCC_DXT5: 
	    	format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT; 
	    	break; 
	    default:
            delete[] (uint8_t*)m_image;
	    	return GL_FALSE; 
	    }
        uint8_t* buffer = &((uint8_t*)m_image)[124];
        unsigned int blockSize = (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16; 
	    unsigned int offset = 0;
	    for (unsigned int level = 0; level < mipMapCount && (width || height); ++level) 
	    { 
	    	unsigned int size = ((width+3)/4)*((height+3)/4)*blockSize; 
	    	glCompressedTexImage2D(GL_TEXTURE_2D, level, format, width, height, 0, size, buffer + offset); 
    
	    	offset += size; 
	    	width /= 2; 
	    	height /= 2; 

	    	if(width < 1)
                width = 1;
	    	if(height < 1) 
                height = 1;
	    } 
        delete[] (uint8_t*)m_image;
	    return GL_TRUE; 
    }
    GLint Texture::BindOtherFormatTexture()
    {
        // Load the texture through stb_image.
        int width = 0, height = 0;
        void* pixels = 
            stbi_load_from_memory((uint8_t*)m_image, m_szImage,
             &width, &height, nullptr,
             STBI_rgb_alpha);
        if (!pixels)
            return GL_FALSE;
        glBindTexture(GL_TEXTURE_2D, m_textureObject);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

        // Set trilinear filtering.
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	    glGenerateMipmap(GL_TEXTURE_2D);
        
        // Free the image.
        stbi_image_free(pixels);
        delete[] (uint8_t*)m_image;
        
        return GL_TRUE;
    }
    GLint Texture::Bind(VAO& to)
    {
        if (!m_image || !m_initialized)
            return GL_FALSE;
    
        to.Bind();
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, m_textureCoordinates.size()*sizeof(GLfloat), m_textureCoordinates.data(), GL_STATIC_DRAW);
        // Bind the texture.
        
        GLint status = GL_TRUE;
        if (m_isDDSImage)
            status = BindDDSTexture();
        else
            status = BindOtherFormatTexture();

        m_vao = &to;
        add_to_vao();
        
        return GL_TRUE;
    }
    GLint Texture::Render()
    {
        if (!m_initialized)
            return GL_FALSE;
        if (!m_vao)
            return GL_FALSE;
        // m_vao->Bind();
        glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_textureObject);
		glUniform1i(m_textureSamplerUniform, 0);
        // Send the UVs to the shader.
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        // glEnableVertexAttribArray(m_vaaIndex);
        glVertexAttribPointer(m_vaaIndex, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
        // glDisableVertexAttribArray(m_vaaIndex);
        return GL_TRUE;
    }
    Texture::~Texture() 
    {
        if (m_initialized)
        {
            if (m_vao)
                remove_from_vao();
            glDeleteBuffers(1, &m_vbo);
            glDeleteTextures(1, &m_textureObject);
        }
    }
}