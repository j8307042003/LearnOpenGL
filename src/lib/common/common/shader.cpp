
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <GL/glew.h>

#include "Shader.h"


namespace common {


	Shader::Shader(const GLchar * vertexShaderPath, const GLchar * fragmentShaderPath, const GLchar * geometryShaderPath) {
		// shader code 
		std::string vertexCode;
		std::string fragmentCode;
		std::string geometryCode;

		// file stream
		std::ifstream vertexFile;
		std::ifstream fragmentFile;
		std::ifstream geometryFile;

		// ensures ifstream objects can throw exceptions:
		vertexFile.exceptions(std::ifstream::badbit);
		fragmentFile.exceptions(std::ifstream::badbit);
		geometryFile.exceptions(std::ifstream::badbit);

		try {
			vertexFile.open(vertexShaderPath);
			fragmentFile.open(fragmentShaderPath);

			std::stringstream vShaderStream, fShaderStream;

			vShaderStream << vertexFile.rdbuf();
			fShaderStream << fragmentFile.rdbuf();

			vertexFile.close();
			fragmentFile.close();

			vertexCode = vShaderStream.str();
			fragmentCode = fShaderStream.str();

			if (geometryShaderPath != nullptr) {
				geometryFile.open(geometryShaderPath);

				std::stringstream gShaderStream;

				gShaderStream << geometryFile.rdbuf();

				geometryFile.close();

				geometryCode = gShaderStream.str();
			}

		}
		catch (std::ifstream::failure f) {
			std::cout << "shader read fail exception!" << std::endl;
		}

		const GLchar* vShaderCode = vertexCode.c_str();
		const GLchar* fShaderCode = fragmentCode.c_str();
		const GLchar* gShaderCode = geometryCode.c_str();

		GLuint vertex, fragment, geometry;
		GLint success;
		GLchar infoLog[512];

		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vShaderCode, NULL);
		glCompileShader(vertex);

		// Print compile errors if any
		glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(vertex, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
		};

		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fShaderCode, NULL);
		glCompileShader(fragment);

		// Print compile errors if any
		glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(fragment, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::fragment::COMPILATION_FAILED\n" << infoLog << std::endl;
		};

		if (geometryShaderPath != nullptr) {
			geometry = glCreateShader(GL_GEOMETRY_SHADER);

			glShaderSource(geometry, 1, &gShaderCode, NULL);
			glCompileShader(geometry);

			glGetShaderiv(geometry, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(geometry, 512, NULL, infoLog);
				std::cout << "ERROR::SHADER::geometry::COMPILATION_FAILED\n" << infoLog << std::endl;
			};

		}


		this->program = glCreateProgram();
		glAttachShader(this->program, vertex);
		glAttachShader(this->program, fragment);
		if (geometryShaderPath != nullptr ) glAttachShader(this->program, geometry);
		glLinkProgram(this->program);
		
		glGetProgramiv(this->program, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(this->program, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;

		}

		// Delete the shaders as they're linked into our program now and no longer necessery
		glDeleteShader(vertex);
		glDeleteShader(fragment);
		if ( geometryShaderPath != nullptr )
			glDeleteShader(geometry);


	}


	void Shader::use() { glUseProgram(this->program); }
}