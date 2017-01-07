#ifndef SHADER_H
#define SHADER_H

#include "GLFW\glfw3.h"



namespace common {
	class Shader {
	public:
		// program location
		GLuint program;
		Shader(const GLchar * vertexShaderPath, const GLchar * fragmentShaderPath);
		void use();

	private:



	};
}
#endif