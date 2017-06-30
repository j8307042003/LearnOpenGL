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

		void SetFloat( const std::string name, float value ){
			glUniform1f(glGetUniformLocation(program, name.c_str()), value); 
		}
		void SetBool(const std::string name, bool value){
			glUniform1i(glGetUniformLocation(program, name.c_str()), (int)value); 
		}
		void SetInt(const std::string name, int value){
			glUniform1i(glGetUniformLocation(program, name.c_str()), value); 
		}
		void SetVec2(const std::string name, const glm::vec2 & value){
 			glUniform2fv(glGetUniformLocation(program, name.c_str()), 1, &value[0]); 
		}
		void SetVec2(const std::string name, float x, float y) {
 			glUniform2f(glGetUniformLocation(program, name.c_str()), x, y); 			
		}
		void SetVec3(const std::string name, const glm::vec3 & value){
 			glUniform3fv(glGetUniformLocation(program, name.c_str()), 1, &value[0]); 			
		}
		void SetVec3(const std::string name, float x, float y, float z) {
 			glUniform3f(glGetUniformLocation(program, name.c_str()), x, y, z); 						
		}
		void SetVec4(const std::string name, const glm::vec4 & value){
 			glUniform4fv(glGetUniformLocation(program, name.c_str()), 1, &value[0]); 						
		}
		void SetVec4(const std::string name, float x, float y, float z, float w) {
 			glUniform4f(glGetUniformLocation(program, name.c_str()), x, y, z, w);
		}
		void SetMat2(const std::string name, const glm::mat2 & mat) {
			glUniformMatrix2fv(glGetUniformLocation(program, name.c_str()), 1, GL_FALSE, &mat[0][0]);
		}
		void SetMat3(const std::string name, const glm::mat3 & mat) {
			 glUniformMatrix3fv(glGetUniformLocation(program, name.c_str()), 1, GL_FALSE, &mat[0][0]);
		}
		void SetMat4(const std::string name, const glm::mat4 & mat) {
			 glUniformMatrix4fv(glGetUniformLocation(program, name.c_str()), 1, GL_FALSE, &mat[0][0]);
		}
	private:
	};
}
#endif