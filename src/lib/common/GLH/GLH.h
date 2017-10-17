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
		operator GLuint() const;
	private:

	};

	class CubeTexture : public Texture {
	public:
		CubeTexture();
		CubeTexture(const char * right, const char * left, const char * top, const char * bottom, const char * back, const char * front);
	};
}
#endif