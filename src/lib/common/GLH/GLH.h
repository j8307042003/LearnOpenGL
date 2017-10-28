#ifndef GLH_H
#define GLH_H

#pragma once

#include "GLFW\glfw3.h"
#include "SOIL\SOIL.h"
#include "stb\stb_image.h"

#include <glm/glm.hpp>


namespace GLH {
#define CubeMapSize 6

	class Texture {
	public:
		unsigned int obj;

		int width, height;

		GLenum wrapS			= GL_REPEAT;
		GLenum wrapT			= GL_REPEAT;
		GLenum internalFormat	= GL_RGB;
		GLenum dataFormat		= GL_RGB;
		GLenum min_fliter		= GL_LINEAR_MIPMAP_LINEAR;
		GLenum mag_fliter		= GL_LINEAR;

		Texture();
		~Texture();
		virtual void load(char * path);
		virtual void load(char * path, GLenum wraps, GLenum wrapt, GLenum internalFormat, GLenum dataFormat, GLenum min_fliter, GLenum mag_fliter);
		virtual void load(unsigned char * data, int width, int height);
		virtual void load( unsigned char * data, int width, int height, GLenum wraps, GLenum wrapt, GLenum internalFormat, GLenum dataFormat, GLenum min_fliter, GLenum mag_fliter );

		void unload();

		virtual void DoLoad( unsigned char * data );
		virtual void DoUnload();

		inline void SetArguments(GLenum wraps, GLenum wrapt, GLenum internalFormat, GLenum dataFormat, GLenum min_fliter, GLenum mag_fliter);

		operator GLuint() const;
	private:

	};

	class CubeTexture : public Texture {
	public:

		struct LoadImage {
			unsigned char * data;
			int width;
			int height;
			LoadImage(unsigned char * data, int width, int height) : data(data), width(width), height(height) {}
		};

		GLenum wrapR = GL_CLAMP_TO_EDGE;

		CubeTexture();
		~CubeTexture();

		void load(const char * right, const char * left, const char * top, const char * bottom, const char * back, const char * front);
		void load(const char * right, const char * left, const char * top, const char * bottom, const char * back, const char * front, GLenum wraps, GLenum wrapt, GLenum internalFormat, GLenum dataFormat, GLenum min_fliter, GLenum mag_fliter);

		void load(unsigned char * right_data, unsigned char * left_data, unsigned char * top_data, unsigned char * bottom_data, unsigned char * back_data, unsigned char * front_data, int width, int height);
		void load(unsigned char * right_data, unsigned char * left_data, unsigned char * top_data, unsigned char * bottom_data, unsigned char * back_data, unsigned char * front_data, int width, int height, GLenum wraps, GLenum wrapt, GLenum internalFormat, GLenum dataFormat, GLenum min_fliter, GLenum mag_fliter);

		void DoLoad( std::vector<LoadImage> cubeImgs );
	};

	enum TextureFormat
	{
		Depth = GL_DEPTH_COMPONENT,
		Stencil = GL_STENCIL_INDEX,
		Depth_Stencil = GL_DEPTH_STENCIL,
		RGB = GL_RGB,
		SRGB = GL_SRGB,
		RGBA = GL_RGBA,
		SRGBA = GL_SRGB_ALPHA
	};

	class FrameBuffer : public Texture {
		public :
			TextureFormat format;
			unsigned int width, height;
			FrameBuffer();
			FrameBuffer(TextureFormat format, unsigned int width, unsigned int height);
			void DoLoad();
	};
}
#endif