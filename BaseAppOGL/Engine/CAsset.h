#pragma once
#include <string>
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <GL/glew.h>
#include <glm/glm.hpp>

#include "Defines.h"
#include "stb_image.h"

//struct Vertex
//{
//    glm::vec3 Position;
//    glm::vec3 Normal;
//    glm::vec2 TexCoords;
//    glm::vec3 Tangent;
//    glm::vec3 Bitangent;
//};
//
//struct Texture
//{
//    unsigned int id;
//    std::string type;
//    std::string path;
//};


class Mesh 
{
public:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;
    unsigned int VAO;

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
    void Draw(unsigned int shaderProgram);
    void DrawWithShadows(unsigned int shaderProgram);
    

private:
    unsigned int VBO, EBO;
    void setupMesh();
};

class CAsset 
{
public:
    CAsset(const std::string& path, bool _flipTexture = false);
    ~CAsset();

    void Draw(unsigned int shaderProgram, bool shadows = false);

    unsigned int numVertices;
    unsigned int numIndices;
    unsigned int numFaces;

	std::vector<Vertex> GetVertices() const { return meshes[0].vertices; }
	std::vector<unsigned int> GetIndices() const { return meshes[0].indices; }

private:
    std::vector<Mesh> meshes;
    std::string directory;
    bool flipTexture;

    void loadModel(const std::string& path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
    unsigned int TextureFromFile(const char* path, const std::string& directory);
};


