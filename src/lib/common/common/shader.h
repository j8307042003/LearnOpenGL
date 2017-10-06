#ifndef SHADER_H
#define SHADER_H

#include "GLFW\glfw3.h"

#include <glm/glm.hpp>


namespace common {
	class Shader {
	public:
		// program location
		GLuint program;
		Shader(const GLchar * vertexShaderPath, const GLchar * fragmentShaderPath, const GLchar * geometryShaderPath = nullptr);
		void use();

		void SetFloat( const std::string name, float value ) const {
			glUniform1f(glGetUniformLocation(program, name.c_str()), value); 
		}
		void SetBool(const std::string name, bool value) const {
			glUniform1i(glGetUniformLocation(program, name.c_str()), (int)value); 
		}
		void SetInt(const std::string name, int value) const {
			glUniform1i(glGetUniformLocation(program, name.c_str()), value); 
		}
		void SetVec2(const std::string name, const glm::vec2 & value) const {
 			glUniform2fv(glGetUniformLocation(program, name.c_str()), 1, &value[0]); 
		}
		void SetVec2(const std::string name, float x, float y) const {
 			glUniform2f(glGetUniformLocation(program, name.c_str()), x, y); 			
		}
		void SetVec3(const std::string name, const glm::vec3 & value) const {
 			glUniform3fv(glGetUniformLocation(program, name.c_str()), 1, &value[0]); 			
		}
		void SetVec3(const std::string name, float x, float y, float z) const {
 			glUniform3f(glGetUniformLocation(program, name.c_str()), x, y, z); 						
		}
		void SetVec4(const std::string name, const glm::vec4 & value) const {
 			glUniform4fv(glGetUniformLocation(program, name.c_str()), 1, &value[0]); 						
		}
		void SetVec4(const std::string name, float x, float y, float z, float w)  const {
 			glUniform4f(glGetUniformLocation(program, name.c_str()), x, y, z, w);
		}
		void SetMat2(const std::string name, const glm::mat2 & mat) const {
			glUniformMatrix2fv(glGetUniformLocation(program, name.c_str()), 1, GL_FALSE, &mat[0][0]);
		}
		void SetMat3(const std::string name, const glm::mat3 & mat) const {
			 glUniformMatrix3fv(glGetUniformLocation(program, name.c_str()), 1, GL_FALSE, &mat[0][0]);
		}
		void SetMat4(const std::string name, const glm::mat4 & mat) const {
			 glUniformMatrix4fv(glGetUniformLocation(program, name.c_str()), 1, GL_FALSE, &mat[0][0]);
		}
	private:
	};
}
#endif