
#pragma once
#define GLEW_STATIC
#include <GL/glew.h>
#include "GLH.h"
#include "SOIL\SOIL.h"
#include "stb\stb_image.h"


// GLFW
#include <GLFW/glfw3.h>

GLH::Texture::Texture():obj(0)
{
}

GLH::Texture::~Texture()
{
	unload();
}

void GLH::Texture::load(char * path)
{
	
	unsigned char * data = stbi_load(path, &width, &height, 0, SOIL_LOAD_RGB);
	load(data, width, height);
	if (data) stbi_image_free(data);
}

void GLH::Texture::load(char * path, GLenum wraps, GLenum wrapt, GLenum internalFormat = GL_RGB, GLenum dataFormat = GL_RGB, GLenum min_fliter = GL_LINEAR_MIPMAP_LINEAR, GLenum mag_fliter = GL_LINEAR)
{
	SetArguments(wraps, wrapt, internalFormat, dataFormat, min_fliter, mag_fliter);
	load(path);
}

void GLH::Texture::load(unsigned char * data, int width, int height)
{
	this->width = width;
	this->height = height;
	DoLoad( data );
}

void GLH::Texture::load(unsigned char * data, int width, int height, GLenum wraps, GLenum wrapt, GLenum internalFormat = GL_RGB, GLenum dataFormat = GL_RGB, GLenum min_fliter = GL_LINEAR_MIPMAP_LINEAR, GLenum mag_fliter = GL_LINEAR)
{
	SetArguments(wraps, wrapt, internalFormat, dataFormat, min_fliter, mag_fliter);
	load(data, width, height);
}

void GLH::Texture::unload()
{
	DoUnload();
}

void GLH::Texture::DoLoad( unsigned char * data )
{
	if (this->obj != 0)
		unload();

	unsigned int textureId;
	glGenTextures(1, &textureId);

	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_fliter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_fliter);

	obj = textureId;
}

void GLH::Texture::DoUnload()
{
	if (this->obj > 0) {
		glDeleteTextures(1, &(this->obj));
		obj = 0;
	}
}

void GLH::Texture::SetArguments(GLenum wraps, GLenum wrapt, GLenum internalFormat, GLenum dataFormat, GLenum min_fliter, GLenum mag_fliter)
{
	this->wrapS = wraps;
	this->wrapT = wrapt;
	this->internalFormat = internalFormat;
	this->dataFormat = dataFormat;
	this->min_fliter = min_fliter;
	this->mag_fliter = mag_fliter;
}

GLH::Texture::operator GLuint() const
{
	return this->obj;
}

GLH::CubeTexture::CubeTexture(): wrapR(GL_CLAMP_TO_EDGE)
{
	wrapS = GL_CLAMP_TO_EDGE;
	wrapT = GL_CLAMP_TO_EDGE;
	mag_fliter = GL_LINEAR;
	min_fliter = GL_LINEAR;
}

GLH::CubeTexture::~CubeTexture()
{
	unload();
}

void GLH::CubeTexture::load(const char * right, const char * left, const char * top, const char * bottom, const char * back, const char * front)
{
	std::vector<std::string> imgPath = { right, left, top, bottom, back, front };
	std::vector<LoadImage>  cubeImgs;
	cubeImgs.reserve(CubeMapSize);

	int loadedWidth, loadedHeight;
	unsigned char * image;
	for (int i = 0; i < CubeMapSize; i++) {
		image = nullptr;
		image = SOIL_load_image(imgPath[i].c_str(), &loadedWidth, &loadedHeight, 0, SOIL_LOAD_RGB);
		cubeImgs.push_back(LoadImage(image, loadedWidth, loadedHeight));
	}
	DoLoad(cubeImgs);

	// release image
	for (int i = 0; i < CubeMapSize; i++) {
		if (cubeImgs[i].data) stbi_image_free(cubeImgs[i].data);
	}
}

void GLH::CubeTexture::load(const char * right, const char * left, const char * top, const char * bottom, const char * back, const char * front, GLenum wraps, GLenum wrapt, GLenum internalFormat = GL_RGB, GLenum dataFormat = GL_RGB, GLenum min_fliter = GL_LINEAR, GLenum mag_fliter = GL_LINEAR)
{
	SetArguments(wraps, wrapt, internalFormat, dataFormat, min_fliter, mag_fliter);
	load(right, left,  top, bottom, back, front);
}

void GLH::CubeTexture::load(unsigned char * right_data, unsigned char * left_data, unsigned char * top_data, unsigned char * bottom_data, unsigned char * back_data, unsigned char * front_data, int width, int height)
{

	std::vector<LoadImage> cubeImgs = { LoadImage( right_data, width, height ), LoadImage(left_data, width, height), LoadImage(top_data, width, height), LoadImage(bottom_data, width, height), LoadImage(back_data, width, height), LoadImage(front_data, width, height) };
	DoLoad(cubeImgs);
}

void GLH::CubeTexture::load(unsigned char * right_data, unsigned char * left_data, unsigned char * top_data, unsigned char * bottom_data, unsigned char * back_data, unsigned char * front_data, int width, int height, GLenum wraps, GLenum wrapt, GLenum internalFormat = GL_RGB, GLenum dataFormat = GL_RGB, GLenum min_fliter = GL_LINEAR, GLenum mag_fliter = GL_LINEAR)
{
	SetArguments(wraps, wrapt, internalFormat, dataFormat, min_fliter, mag_fliter);
	load(right_data, left_data, top_data, bottom_data, back_data, front_data, width, height);
}

void GLH::CubeTexture::DoLoad(std::vector<LoadImage> cubeImgs)
{
	if (cubeImgs.size() != CubeMapSize) {
		std::cout << " cube map size not correct. need : " << CubeMapSize << ". receieved : " << cubeImgs.size() << "\n";
		return;
	}

	if (obj != 0)
		unload();

	GLuint textureID;
	glGenTextures(1, &textureID);

	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	for (int i = 0; i < cubeImgs.size(); ++i) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, cubeImgs[i].width, cubeImgs[i].height, 0, GL_RGB, GL_UNSIGNED_BYTE, cubeImgs[i].data);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, mag_fliter);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, min_fliter);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, wrapS);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, wrapT);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, wrapR);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	this->obj = textureID;
}

GLH::FrameBuffer::FrameBuffer()
{
}

GLH::FrameBuffer::FrameBuffer(TextureFormat format, unsigned int width, unsigned int height):format(format), width(width), height(height){

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
