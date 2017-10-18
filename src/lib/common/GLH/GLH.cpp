
#pragma once
#define GLEW_STATIC
#include <GL/glew.h>
#include "GLH.h"
#include "SOIL\SOIL.h"
#include "stb\stb_image.h"


// GLFW
#include <GLFW/glfw3.h>

GLH::Texture::Texture()
{
}

GLH::Texture::Texture(const char * path, const bool & gammaCorrection) :gammaCorrection(gammaCorrection), path(NULL), obj(0)
{
	size_t length = strlen(path) + 1;
	char * str = new char[length];

	this->path = str ;
	strcpy(this->path, path);
	load();
}

GLH::Texture::~Texture()
{
	unload();
	delete[] path;
}

void GLH::Texture::load()
{
	DoLoad();
}

void GLH::Texture::unload()
{
	DoUnload();
}

void GLH::Texture::DoLoad()
{
	if (this->obj != 0 || path == NULL) return;

	unsigned int textureId;
	glGenTextures(1, &textureId);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);

	if (data)
	{
		GLenum internalFormat;
		GLenum dataFormat;
		if (nrComponents == 1)
		{
			internalFormat = dataFormat = GL_RED;
		}
		else if (nrComponents == 3)
		{
			internalFormat = gammaCorrection ? GL_SRGB : GL_RGB;
			dataFormat = GL_RGB;
		}
		else if (nrComponents == 4)
		{
			internalFormat = gammaCorrection ? GL_SRGB_ALPHA : GL_RGBA;
			dataFormat = GL_RGBA;
		}

		glBindTexture(GL_TEXTURE_2D, textureId);
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		obj = textureId;
		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}
}

void GLH::Texture::DoUnload()
{
	if (this->obj > 0) {
		glDeleteTextures(1, &(this->obj));
		obj = 0;
	}
}

GLH::Texture::operator GLuint() const
{
	return this->obj;
}

GLH::CubeTexture::CubeTexture()
{
}

GLH::CubeTexture::CubeTexture(const char * right, const char * left, const char * top, const char * bottom, const char * back, const char * front):cubeMapPath(6)
{

	cubeMapPath[0] = right;
	cubeMapPath[1] = left;
	cubeMapPath[2] = top;
	cubeMapPath[3] = bottom;
	cubeMapPath[4] = back;
	cubeMapPath[5] = front;

	load();

}

void GLH::CubeTexture::DoLoad()
{

	GLuint textureID;
	glGenTextures(1, &textureID);
	glActiveTexture(GL_TEXTURE0);

	int width, height;
	unsigned char * image;

	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	for (int i = 0; i < cubeMapPath.size(); i++) {
		image = SOIL_load_image(cubeMapPath[i].c_str(), &width, &height, 0, SOIL_LOAD_RGB);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		SOIL_free_image_data(image);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	this->obj = textureID;


}

GLH::FrameBuffer::FrameBuffer()
{
}

GLH::FrameBuffer::FrameBuffer(TextureFormat format, unsigned int width, unsigned int height):format(format), width(width), height(height)
{
	load();
}

void GLH::FrameBuffer::DoLoad()
{
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, format == TextureFormat::Depth_Stencil?(GL_DEPTH24_STENCIL8): format, width, height, 0, format, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	this->obj = textureID;
}
