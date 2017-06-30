

#pragma once
// Std. Includes
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>



// GL Includes
#include <GL/glew.h> // Contains all the necessery OpenGL includes
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.h"
#include"assimp\types.h"

namespace common {
	struct Vertex {
		// Position
		glm::vec3 Position;
		// Normal
		glm::vec3 Normal;
		// TexCoords
		glm::vec2 TexCoords;
	};


	struct Texture
	{
		GLuint id;
		std::string type;
		aiString path;
	};


	class Mesh {
	public:
		std::vector<Vertex> vertices;
		std::vector<Texture> textures;
		std::vector<GLuint> indices;


		Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<Texture> textures);
		void Draw(Shader shader);
		GLuint  GetVAO();
	private:
		GLuint VAO, VBO, EBO;

		void setupMesh();
	};


}