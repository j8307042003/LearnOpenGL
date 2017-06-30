
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
#include "Mesh.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace common {
	class Model
	{
	public:
		/*  Functions   */
		Model(GLchar* path);
		std::vector<Mesh> meshes;
		std::vector<Texture> textures_loaded;

		void Draw(Shader shader);
	private:
		/*  Model Data  */
		std::string directory;
		/*  Functions   */
		void loadModel(std::string path);
		void processNode(aiNode* node, const aiScene* scene);
		Mesh processMesh(aiMesh* mesh, const aiScene* scene);
		std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type,
			std::string typeName);
		GLuint TextureFromFile(const char* path, std::string directory);
	};
}