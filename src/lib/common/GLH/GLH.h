#ifndef GLH_H
#define GLH_H

#pragma once

#include "GLFW\glfw3.h"
#include "SOIL\SOIL.h"
#include "stb\stb_image.h"

#include <glm/glm.hpp>


namespace GLH {
	class Texture {
	public:
		unsigned int obj;
		char * path;
		bool gammaCorrection;
		Texture();
		Texture(const char * path, const bool & gammaCorrection = true);
		~Texture();
		void load();
		void unload();
		virtual void DoLoad();
		virtual void DoUnload();
		operator GLuint() const;
	private:

	};

	class CubeTexture : public Texture {
	public:

		std::vector<std::string> cubeMapPath;

		CubeTexture();
		CubeTexture(const char * right, const char * left, const char * top, const char * bottom, const char * back, const char * front);
		void DoLoad();
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